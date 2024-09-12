#include <Windows.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <errhandlingapi.h>
#include <handleapi.h>
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
        try
        {
            UpdateErrorLogBuffer();
        }
        catch (CSOL24H_EXCEPT e)
        {
            ConsoleLog("控制器发生运行时错误：%s。", ENUM_CONSOLE_LOG_LEVEL::CLL_ERROR, e.what());
            break;
        }
        TransferGameState();
        DispatchCommand();
        Sleep(100);
    }
    ConsoleLog("线程 hWatchInGameStateThread 退出。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE);
    return 0;
}

/*
@brief 从日志中解析游戏状态
*/
void CSOL24H::ResolveGameStateFromErrorLog() noexcept
{
    int64_t begin = cbGameErrorLogBufferSize;
    int64_t end = cbGameErrorLogBufferSize;
    int64_t current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(); /* 获取时间戳 */
    int64_t log_timestamp;
    std::string line;
    const char* msg = nullptr;
    bool bHasGameStateUpdated = false;
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
        log_timestamp = ResolveMessageTimestamp(line, nullptr); /* 当前正解析的这条游戏日志时间戳 */
        /* 按行解析日志消息并下达命令 */
        if (line.find("Join HostServer") != std::string::npos)
        {
            bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_LOADING, log_timestamp);
            msg = "连接到服务器主机，加载游戏场景";
            break;
        }
        else if (line.find("Result Confirm") != std::string::npos)
        {
            bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_IN_ROOM, log_timestamp);
            msg = "游戏结算，回到房间";
            return_to_room_times = 0; /* 正常完成一局游戏，清零 */
            break;
        }
        else if (line.find("S_ROOM_ENTER") != std::string::npos)
        {
            bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_IN_ROOM, log_timestamp);
            msg = "进入游戏房间";
            return_to_room_times = 0; /* 进入新房间，清零 */
            break;
        }
        else if (line.find("Leave Game Room") != std::string::npos)
        {
            bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_IN_HALL, log_timestamp);
            msg = "返回到游戏大厅";
            break;
        }
        else if (line.find("Return to room") != std::string::npos) /* 由于异常原因返回到游戏房间内 */
        {
            bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_IN_ROOM, log_timestamp);
            msg = "因异常原因回到游戏房间";
            if (bHasGameStateUpdated) /* 更新成功，说明此状态是有效的新状态 */
            {
                return_to_room_times++; /* 回到房间的次数增加 */
            }
            break;
        }
        else if (line.find("QuitLog: game shutdown") != std::string::npos)
        {
            bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_SHUTDOWN, log_timestamp);
            msg = "游戏退出";
            break;
        }
        else if (line.find("Game Server Login Success") != std::string::npos)
        {
            bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_LOGIN, log_timestamp);
            msg = "成功登录游戏";
            break;
        }
        else if (line.find("Log File Opened") != std::string::npos)
        {
            bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_UNKNOWN, log_timestamp);
            msg = "等待游戏客户端加载";
            break;
        }
        else
        {
            continue;
        }
    }
    if (bHasGameStateUpdated)
    {
        ConsoleLog("%s。游戏日志时间戳：%lld，当前时间戳：%lld。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, msg, log_timestamp, current_time);
    }
    bGameErrorLogBufferResolved = true;
}
/*
@brief 对游戏状态例行检查
*/
void CSOL24H::CheckGameState() noexcept
{
    int64_t current_time;
    int64_t log_timestamp;
    const char* msg = nullptr;
    bool bHasGameStateUpdated = false;
    current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(); /* 获取时间戳 */
    if (in_game_state.get_state() == ENUM_IN_GAME_STATE::IGS_LOADING && current_time - in_game_state.get_timestamp() > 25) /* 加载时间达到 25 秒 */
    {
        bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_IN_MAP, current_time); /* 状态由 LOADING 转为 MAP，时间戳更新为当前时刻 */
        msg =  "预设加载时间已过，认为已经进入游戏场景";
    }
    else if (in_game_state.get_state() == ENUM_IN_GAME_STATE::IGS_LOGIN && current_time - in_game_state.get_timestamp() > 30) /* 登陆后等待 30 秒 */
    {
        if (IsWindow(hGameWindow))
        {
            ShowWindow(hGameWindow, SW_NORMAL);
            auto TopmostWindow = reinterpret_cast<void(*)(HWND)>(GetProcAddress(hGamingToolModule, "TopmostWindow"));
            TopmostWindow(hGameWindow);
            ConsoleLog("将游戏窗口置顶。使用 GamingTool 可解除（Win + T），详见 GamingTool 使用说明。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE);
            // SetForegroundWindow(hGameWindow);
            // DWORD_PTR _; /* dummy */
            // SendMessageTimeout(hGameWindow, WM_NULL, 0, 0, SMTO_NORMAL, 5000, &_);
            // if (GetForegroundWindow() == hGameWindow)
            // {
            //     ConsoleLog("成功将游戏窗口置于前台并激活。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE);
            // }
            // else
            // {
            //     ConsoleLog("未能成功将游戏窗口置于前台并激活。", ENUM_CONSOLE_LOG_LEVEL::CLL_WARNING);
            // }
            Sleep(500);
            auto MakeWindowBorderless = reinterpret_cast<void(*)(HWND)>(GetProcAddress(hGamingToolModule, "MakeWindowBorderless"));
            if (MakeWindowBorderless)
            {
                MakeWindowBorderless(hGameWindow);
                ConsoleLog("去除游戏窗口标题栏。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE);
            }
        }
        bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_IN_HALL, current_time); /* 状态由 LOGIN 转为 HALL，时间戳更新为当前时刻 */
        msg = "游戏进程启动后等待时间达到 30 秒，认为游戏客户端已经完全加载";
    }
    else if (in_game_state.get_state() == ENUM_IN_GAME_STATE::IGS_IN_ROOM && current_time - in_game_state.get_timestamp() > 15 * 60) /* 在房间内等待 15 分钟而未开始游戏 */
    {
        bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_IN_HALL, current_time); /* 状态由 ROOM 转为 HALL，时间戳更新为当前时刻 */
        msg = "在房间内等待时间超过 15 分钟（等待时间过长的房间将自动关闭），认为此房间将要关闭，返回到大厅";
    }
    else if (in_game_state.get_state() == ENUM_IN_GAME_STATE::IGS_IN_ROOM && return_to_room_times > 3) /* 因异常原因返回到房间次数过多 */
    {
        bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL, current_time);
        msg = "因异常原因返回到房间内次数过多（如：重连 1 2 3 问题、游戏时网络波动），认为此房间无法正常游戏，返回到大厅";
    }
    else if (in_game_state.get_state() == ENUM_IN_GAME_STATE::IGS_IN_MAP && current_time - in_game_state.get_timestamp() > 30 * 60) /* 在地图中游戏时间超过 30 分钟（异常情形） */
    {
        bHasGameStateUpdated = in_game_state.update(ENUM_IN_GAME_STATE::IGS_IN_HALL, current_time);
        msg = "长时间未能进行游戏结算确认，返回到大厅";
    }
    if (bHasGameStateUpdated)
    {
        ConsoleLog("%s。游戏日志时间戳更新为当前时间戳：%lld。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, msg, current_time);
    }
}

/*
@brief 状态迁移函数。
*/
void CSOL24H::TransferGameState() noexcept
{
    /* 日志文件缓冲区更新，则执行解析 */
    if (!bGameErrorLogBufferResolved)
    {
        ResolveGameStateFromErrorLog();
    }
    /* 例行检查 */
    else
    {
        CheckGameState();
    }
}

/*
@brief 更新日志文件缓冲区。
@return 日志文件缓冲区是否更新。
*/
void CSOL24H::UpdateErrorLogBuffer()
{
    DWORD dwBytesWritten;
    FILETIME file_time;
    GetFileTime(hGameErrorLogFile, nullptr, nullptr, (LPFILETIME)&file_time);
    int64_t file_last_modified_time = (int64_t)file_time.dwLowDateTime | (int64_t)file_time.dwHighDateTime << 32; /* 文件时间本质实际上是 64 位数 */
    if (log_buffer_last_modified_time != file_last_modified_time) /* 文件已经被修改，需要加载 */
    {
        log_buffer_last_modified_time = file_last_modified_time;
        bGameErrorLogBufferResolved = false;
        LARGE_INTEGER cbNewLogSize;
        GetFileSizeEx(hGameErrorLogFile, &cbNewLogSize);
        if (cbNewLogSize.HighPart != 0) /* 超过大小限制 */
        {
            throw CSOL24H_EXCEPT("日志大小超出限制，请重启游戏和控制器，并向开发者报告此问题。");
        }
        LARGE_INTEGER file_pointer = { .QuadPart = 0 }; /* 文件指针位置 */
        SetFilePointerEx(hGameErrorLogFile, file_pointer, &file_pointer, FILE_CURRENT);
        if (cbNewLogSize.QuadPart > file_pointer.QuadPart) /* 文件变大，则说明只是向 Error.log 追加新内容，故只需要读取新增部分 */
        {
            ReadFile(
                hGameErrorLogFile,
                lpGameErrorLogBuffer,
                cbNewLogSize.QuadPart - file_pointer.QuadPart, /* 读取新增内容到缓冲区 */
                &dwBytesWritten,
                NULL
            );
        }
        else
        {
            SetFilePointer(
                hGameErrorLogFile,
                0,
                0,
                FILE_BEGIN
            );
            ReadFile(
                hGameErrorLogFile,
                lpGameErrorLogBuffer,
                cbNewLogSize.QuadPart,
                &dwBytesWritten,
                NULL
            );
        }
        cbGameErrorLogBufferSize = dwBytesWritten;
        game_error_log_file_date = ResolveLogDate(file_time); /* 将文件修改的日期作为日志的日期，获取日期的 00:00:00（需要考虑时区） 时刻的时间戳 */
    }
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