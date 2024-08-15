#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <errhandlingapi.h>
#include <iomanip>
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <fileapi.h>
#include <iterator>
#include <libloaderapi.h>
#include <memory>
#include <minwinbase.h>
#include <minwindef.h>
#include <processenv.h>
#include <processthreadsapi.h>
#include <regex>
#include <synchapi.h>
#include <timezoneapi.h>
#include <wincon.h>
#include <winerror.h>
#include <winnt.h>
#include "CSOL24H.hpp"
#include "GameState.hpp"
#include "Util.hpp"
#include "CSOL24H_EXCEPT.hpp"
#include "Command.hpp"
#include "Console.hpp"

DWORD CALLBACK CSOL24H::WatchInGameState(LPVOID lpParam) noexcept
{
    HANDLE hObjectsToWait[] = {
        hEnableWatchInGameStateEvent,
        hGameProcessRunningEvent,
    };
    while (true)
    {
        DWORD dwRet = WAIT_OBJECT_0 + ARRAYSIZE(hObjectsToWait) == WaitForMultipleObjects(
            ARRAYSIZE(hObjectsToWait),
            hObjectsToWait,
            TRUE, /* Wait All */
            INFINITE
        ); /* 获取到所有可等待对象才开始运行 */
        if (dwRet >= ARRAYSIZE(hObjectsToWait)) break;
        if (bDestroy) break;
        UpdateErrorLogBuffer();
        TransferGameState();
        DispatchCommand();
        Sleep(100);
    }
    ConsoleLog("线程 hWatchInGameStateThread 退出。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE);
    return 0;
}

/*
@brief 状态迁移函数。
*/
void CSOL24H::TransferGameState() noexcept
{
    if (cbGameErrorLogSize <= 0) return;
    int64_t begin = cbGameErrorLogSize;
    int64_t end = cbGameErrorLogSize;
    int64_t current_time;
    int64_t log_timestamp;
    std::string line;
    const char* msg = nullptr;
    InGameState gs;
    static int32_t return_to_room_times = 0; /* 因网络问题返回到游戏房间的次数 */
    /* 例行检查 */
    if (bGameErrorLogBufferResolved)
    {
        current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(); /* 获取时间戳 */
        if (in_game_state.get_state() == ENUM_IN_GAME_STATE::IGS_LOADING && std::abs(current_time - in_game_state.get_timestamp()) > 30) /* 加载时间达到 30 秒 */
        {
            gs.update(ENUM_IN_GAME_STATE::IGS_IN_MAP, current_time); /* 状态由 LOADING 转为 MAP，时间戳更新为当前时刻 */
            msg =  "预设加载时间已过，认为已经进入游戏场景";
        }
        if (in_game_state.get_state() == ENUM_IN_GAME_STATE::IGS_LOGIN && std::abs(current_time - in_game_state.get_timestamp()) > 30) /* 登陆后等待 30 秒 */
        {
            if (IsWindow(hGameWindow))
            {
                SetForegroundWindow(hGameWindow);
                DWORD_PTR _; /* dummy */
                SendMessageTimeout(hGameWindow, WM_NULL, 0, 0, SMTO_NORMAL, 5000, &_);
                if (GetForegroundWindow() == hGameWindow)
                {
                    ConsoleLog("成功将游戏窗口置于前台并激活。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE);
                }
                else
                {
                    ConsoleLog("未能成功将游戏窗口置于前台并激活。", ENUM_CONSOLE_LOG_LEVEL::CLL_WARNING);
                }
                auto MakeWindowBorderless = (void(*)(HWND))GetProcAddress(hGamingToolModule, "MakeWindowBorderless");
                if (MakeWindowBorderless) {
                    MakeWindowBorderless(hGameWindow);
                    ConsoleLog("去除游戏窗口标题栏。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE);
                }
            }
            gs.update(ENUM_IN_GAME_STATE::IGS_IN_HALL, current_time); /* 状态由 LOGIN 转为 HALL，时间戳更新为当前时刻 */
            msg = "登陆后等待时间已过，认为游戏客户端已经完全加载";
        }
        if (in_game_state.get_state() == ENUM_IN_GAME_STATE::IGS_IN_ROOM && std::abs(current_time - in_game_state.get_timestamp()) > 15 * 60) /* 在房间内等待 15 分钟而未开始游戏 */
        {
            gs.update(ENUM_IN_GAME_STATE::IGS_IN_HALL, current_time); /* 状态由 ROOM 转为 HALL，时间戳更新为当前时刻 */
            msg = "在房间内久未进入游戏，认为已经回到大厅";
        }
        if (in_game_state.update(gs))
        {
            ConsoleLog("%s。游戏日志时间戳更新为当前时间戳：%lld。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, msg, current_time);
        }
        return;
    }
    /* 自后向前，按照 CRLF 方式划分子串，以文件行为单位解析 buffer 内容 */
    while (begin >= 0)
    {
        if (begin == 0 && begin != end) /* [begin, end) 可划分为子串 */
        {
            line = std::string(lpGameErrorLogBuffer + begin, end - begin);
            begin--; /* begin 减小为负数后退出 */
        }
        else if (lpGameErrorLogBuffer[begin - 1] == '\n' && begin != end) /* [begin, end) 可划分为子串 */
        {
            line = std::string(lpGameErrorLogBuffer + begin, end - begin);
            begin -= 2; /* 越过 \r\n */
            end = begin;
        }
        else if (lpGameErrorLogBuffer[begin - 1] == '\n' && begin == end) /* [begin, end) 长度为 0 */
        {
            begin -= 2;
            end = begin;
            continue;
        }
        else
        {
            begin--;
            continue;
        }
        current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(); /* 获取时间戳 */
        log_timestamp = ResolveMessageTimestamp(line, nullptr);
        /* 按行解析日志消息并下达命令 */
        if (line.find("Join HostServer") != std::string::npos)
        {
            gs.update(ENUM_IN_GAME_STATE::IGS_LOADING, log_timestamp);
            msg = "连接到服务器主机，加载游戏场景";
        }
        else if (line.find("Result Confirm") != std::string::npos)
        {
            gs.update(ENUM_IN_GAME_STATE::IGS_IN_ROOM, log_timestamp);
            msg = "游戏结算，回到房间";
            return_to_room_times = 0; /* 正常完成一局游戏，清零 */
        }
        else if (line.find("S_ROOM_ENTER") != std::string::npos)
        {
            gs.update(ENUM_IN_GAME_STATE::IGS_IN_ROOM, log_timestamp);
            msg = "进入游戏房间";
            return_to_room_times = 0; /* 进入新房间，清零 */
        }
        // else if (line.find("Start Game Room") != std::string::npos)
        // {
        //     gs.update(ENUM_IN_GAME_STATE::GS_LOADING, log_timestamp);
        //     msg = "开始游戏，即将加载游戏场景";
        // }
        else if (line.find("Leave Game Room") != std::string::npos)
        {
            gs.update(ENUM_IN_GAME_STATE::IGS_IN_HALL, log_timestamp);
            msg = "返回到游戏大厅";
        }
        else if (line.find("Return to room") != std::string::npos)
        {
            return_to_room_times++;
            if (return_to_room_times > 3)
            {
                gs.update(ENUM_IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL, current_time);
                msg = "因网络问题返回到房间内次数过多，尝试回到大厅重新创建房间";
            }
            else
            {
                gs.update(ENUM_IN_GAME_STATE::IGS_IN_ROOM, log_timestamp);
                msg = "回到游戏房间";
            }
        }
        else if (line.find("QuitLog: game shutdown") != std::string::npos)
        {
            gs.update(ENUM_IN_GAME_STATE::IGS_SHUTDOWN, log_timestamp);
            msg = "游戏退出";
        }
        else if (line.find("Game Server Login") != std::string::npos)
        {
            gs.update(ENUM_IN_GAME_STATE::IGS_LOGIN, log_timestamp);
            msg = "正在登录客户端";
        }
        else if (line.find("Log File Opened") != std::string::npos)
        {
            gs.update(ENUM_IN_GAME_STATE::IGS_UNKNOWN, log_timestamp);
            msg = "未知状态";
        }
        else
        {
            continue;
        }
        if (in_game_state.update(gs))
        {
            ConsoleLog("%s。游戏日志时间戳：%lld，当前时间戳：%lld。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, msg, log_timestamp, current_time);
        }
        bGameErrorLogBufferResolved = true;
        return;
    }
}


/*
@brief 更新日志文件缓冲区。
@return 日志文件缓冲区是否更新。
*/
bool CSOL24H::UpdateErrorLogBuffer() noexcept
{
    DWORD dwBytesWritten;
    FILETIME filetime;
    GetFileTime(hGameErrorLogFile, nullptr, nullptr, (LPFILETIME)&filetime);
    int64_t file_last_modified_time = (int64_t)filetime.dwLowDateTime | (int64_t)filetime.dwHighDateTime << 32; /* 文件时间本质实际上是 64 位数 */
    if (log_buffer_last_modified_time != file_last_modified_time) /* 文件已经被修改，需要加载 */
    {
        log_buffer_last_modified_time = file_last_modified_time;
        bGameErrorLogBufferResolved = false;
        INT64 cbUpdatedLog = 0;
        DWORD dwNewSizeHigh = 0;
        DWORD dwNewSizeLow = GetFileSize(hGameErrorLogFile, &dwNewSizeHigh);
        cbUpdatedLog = dwNewSizeLow | ((INT64)dwNewSizeHigh << 32);
        if (cbUpdatedLog > cbGameErrorLogSize) /* 文件变大且时间戳非零（初始状态），则说明只是向 Error.log 追加新内容，故只需要读取新增部分 */
        {
            ReadFile(
                hGameErrorLogFile,
                lpGameErrorLogBuffer + cbGameErrorLogSize, /* append to buffer */
                dwNewSizeLow - cbGameErrorLogSize, /* new content */
                &dwBytesWritten,
                nullptr
            );
            cbGameErrorLogSize += dwBytesWritten;
        }
        else
        {
            SetFilePointer(
                hGameErrorLogFile,
                0,
                0,
                FILE_BEGIN
            );
            ReadFile(hGameErrorLogFile, lpGameErrorLogBuffer, dwNewSizeLow, &dwBytesWritten, nullptr);
            cbGameErrorLogSize = dwBytesWritten;
        }
        // qwGameErrorLogFileDateTime = ResolveLogDate(lpGameErrorLogBuffer, cbGameErrorLogSize); /* 更新日志时间（废弃此方法，因为日期变更后若日志文件不刷新则从日志文件解析出的时间不准确） */
        CSOL24H::game_error_log_file_date = ResolveLogDate(filetime); /* 将文件修改的日期作为日志的日期，获取日期的 00:00:00（需要考虑时区） 时刻的时间戳 */
        return true;
    }
    return false;
}

void CSOL24H::DispatchCommand() noexcept
{
    int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(); /* 获取时间戳 */
    static int64_t last_confirm_time = 0;
    bool need_confirm = false;
    const char* cmd = LUA_CMD_NOP;
    auto state = in_game_state.get_state();
    if (state == ENUM_IN_GAME_STATE::IGS_LOADING)
    {
        cmd = LUA_CMD_NOP;
    }
    else if (state == ENUM_IN_GAME_STATE::IGS_IN_MAP)
    {
        if (std::abs(timestamp - in_game_state.get_timestamp()) < 10)
        {
            cmd = LUA_CMD_CHOOSE_CLASS;
        }
        else if (std::abs(timestamp - in_game_state.get_timestamp()) < 120)
        {
            cmd = LUA_CMD_PLAY_GAME_NORMAL;
        }
        else /* 不需要确认，且游戏时间超过 2 分钟，资金充足 */
        {
            cmd = bAllowExtendedMode ? LUA_CMD_PLAY_GAME_EXTEND : LUA_CMD_PLAY_GAME_NORMAL;
        }
    }
    else if (state == ENUM_IN_GAME_STATE::IGS_IN_ROOM)
    {
        cmd = LUA_CMD_START_GAME_ROOM;
    }
    else if (state == ENUM_IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL)
    {
        cmd = LUA_CMD_CREATE_ROOM;
    }
    else if (state == ENUM_IN_GAME_STATE::IGS_IN_HALL)
    {
        cmd = LUA_CMD_CREATE_ROOM;
    }
    GiveCommand(cmd);
}