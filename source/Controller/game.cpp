#include <Windows.h>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <fileapi.h>
#include <iterator>
#include <memory>
#include <minwindef.h>
#include <processenv.h>
#include <processthreadsapi.h>
#include <regex>
#include <synchapi.h>
#include <wincon.h>
#include <winerror.h>
#include <winnt.h>
#include "CSOL24H.hpp"
#include "GameState.hpp"
#include "util.hpp"
#include "CSOL24H_EXCEPT.hpp"
#include "command.hpp"

/*
@brief 按“开始游戏”按钮
*/
// DWORD CALLBACK CSOL24H::StartGameRoom(LPVOID lpParam) noexcept
// {
//     while (WaitForSingleObject(hStartGameEvent, INFINITE))
//     {
//         if (bExit)
//         {
//             break;
//         }
//         CSOL24H::GiveCommand(LUA_CMD_START_GAME_ROOM);
//     }
//     return 0;
// }
/*
@brief 下达游玩命令。
*/
// DWORD CALLBACK CSOL24H::PlayGame(LPVOID lpParam) noexcept
// {
//     while (WaitForSingleObject(hPlayGameEvent, INFINITE))
//     {
//         if (bExit)
//         {
//             break;
//         }
//         if (WAIT_OBJECT_0 == WaitForSingleObject(hGiveCmdMutex, INFINITE))
//             CSOL24H::GiveCommand(LUA_CMD_PLAY_GAME);
//     }
//     return 0;
// }
/*
@brief 确认游戏结算页面。
*/
// DWORD CALLBACK CSOL24H::ConfirmGame(LPVOID lpParam) noexcept
// {
//     while (WaitForSingleObject(hConfirmRoundEvent, INFINITE))
//     {
//         if (bExit)
//         {
//             break;
//         }
//         CSOL24H::GiveCommand(LUA_CMD_TRY_CONFIRM_RESULT);
//     }
//     return 0;
// }

/*
@brief 状态迁移函数。
*/
void CSOL24H::TransferGameState() noexcept
{
    if (cbErrorLogSize <= 0) return;
    static std::regex msg_pattern("(\\d{2}):(\\d{2}):(\\d{2}).(\\d{3}) - (.+)");
    int64_t begin = cbErrorLogSize;
    int64_t end = cbErrorLogSize;
    int64_t current_time;
    int64_t log_timestamp;
    std::string line;
    const char* msg = nullptr;
    GameState gs;
    /* 例行检查 */
    if (bLogBufferResolved)
    {
        current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(); /* 获取时间戳 */
        if (game_state.get_state() == ENUM_GAME_STATE::GS_LOADING && std::abs(current_time - game_state.get_timestamp()) > 30) /* 加载时间达到 30 秒 */
        {
            gs.update(ENUM_GAME_STATE::GS_MAP, current_time); /* 状态由 LOADING 转为 MAP，时间戳更新为当前时刻 */
            msg =  "预设加载时间已过，认为已经进入游戏场景";
        }
        if (game_state.get_state() == ENUM_GAME_STATE::GS_LOGIN && std::abs(current_time - game_state.get_timestamp()) > 15) /* 登陆后等待 15 秒 */
        {
            gs.update(ENUM_GAME_STATE::GS_HALL, current_time); /* 状态由 LOGIN 转为 HALL，时间戳更新为当前时刻 */
            msg = "登陆后等待时间已过，认为游戏客户端已经完全加载";
        }
        if (game_state.update(gs))
        {
            std::printf("【消息】%s。游戏日志时间戳更新为当前时间戳：%lld。\r\n", msg, current_time);
        }
        return;
    }
    /* 自后向前，按照 CRLF 方式划分子串，以文件行为单位解析 buffer 内容 */
    while (begin >= 0)
    {
        if (begin == 0 && begin != end) /* [begin, end) 可划分为子串 */
        {
            line = std::string(lpErrorLogBuffer + begin, end - begin);
            begin--; /* begin 减小为负数后退出 */
        }
        else if (lpErrorLogBuffer[begin - 1] == '\n' && begin != end) /* [begin, end) 可划分为子串 */
        {
            line = std::string(lpErrorLogBuffer + begin, end - begin);
            begin -= 2; /* 越过 \r\n */
            end = begin;
        }
        else if (lpErrorLogBuffer[begin - 1] == '\n' && begin == end) /* [begin, end) 长度为 0 */
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
        if (line.find("Join Host Server") != std::string::npos)
        {
            gs.update(ENUM_GAME_STATE::GS_LOADING, log_timestamp);
            msg = "加载游戏场景";
        }
        else if (line.find("Result Confirm") != std::string::npos)
        {
            gs.update(ENUM_GAME_STATE::GS_ROOM, log_timestamp);
            msg = "游戏结算，回到房间";
        }
        else if (line.find("S_ROOM_ENTER") != std::string::npos)
        {
            gs.update(ENUM_GAME_STATE::GS_ROOM, log_timestamp);
            msg = "进入游戏房间";
        }
        else if (line.find("Start Game Room") != std::string::npos)
        {
            gs.update(ENUM_GAME_STATE::GS_LOADING, log_timestamp);
            msg = "即将加载游戏场景";
        }
        else if (line.find("Leave Game Room") != std::string::npos)
        {
            gs.update(ENUM_GAME_STATE::GS_HALL, log_timestamp);
            msg = "返回到游戏大厅";
        }
        else if (line.find("Return to Room") != std::string::npos)
        {
            gs.update(ENUM_GAME_STATE::GS_ROOM, log_timestamp);
            msg = "回到游戏房间";
        }
        else if (line.find("Game Server Login") != std::string::npos)
        {
            gs.update(ENUM_GAME_STATE::GS_LOGIN, log_timestamp);
            msg = "正在登录客户端";
        }
        else if (line.find("Log File Opened") != std::string::npos)
        {
            gs.update(ENUM_GAME_STATE::GS_UNKNOWN, log_timestamp);
            msg = "未知状态";
        }
        else
        {
            continue;
        }
        if (game_state.update(gs))
        {
            std::printf("【消息】%s。游戏日志时间戳：%lld，当前时间戳：%lld。\r\n", msg, log_timestamp, current_time);
        }
        bLogBufferResolved = true;
        return;
    }
}

DWORD CALLBACK CSOL24H::WatchGameState(LPVOID lpParam) noexcept
{
    while (WAIT_OBJECT_0 == WaitForSingleObject(hGameWatcherEvent, INFINITE))
    {
        if (bExit)
        {
            break;
        }
        UpdateErrorLogBuffer();
        TransferGameState();
        DispatchCommand();
        Sleep(100);
    }
    return 0;
}

void CSOL24H::DispatchCommand() noexcept
{
    int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(); /* 获取时间戳 */
    static int64_t last_confirm_time = 0;
    bool need_confirm = false;
    const char* cmd = LUA_CMD_NOP;
    auto state = game_state.get_state();
    if (state == ENUM_GAME_STATE::GS_LOADING)
    {
        cmd = LUA_CMD_NOP;
    }
    else if (state == ENUM_GAME_STATE::GS_MAP)
    {
        if (std::abs(timestamp - game_state.get_timestamp()) < 10)
        {
            cmd = LUA_CMD_CHOOSE_CLASS;
        }
        else if (std::abs(timestamp - game_state.get_timestamp()) % 30 < 5)
        {
            cmd = LUA_CMD_TRY_CONFIRM_RESULT;
        }
        else if (std::abs(timestamp - game_state.get_timestamp()) < 120)
        {
            cmd = LUA_CMD_PLAY_GAME;
        }
        else /* 不需要确认，且游戏时间超过 2 分钟，资金充足 */
        {
            cmd = LUA_CMD_PLAY_GAME_EXTEND;
        }
    }
    else if (state == ENUM_GAME_STATE::GS_ROOM)
    {
        cmd = LUA_CMD_START_GAME_ROOM;
    }
    GiveCommand(cmd);
}

void CSOL24H::StartWatch() noexcept
{
    SetEvent(hGameWatcherEvent);
}

void CSOL24H::StopWatch() noexcept
{
    ResetEvent(hGameWatcherEvent);
}

