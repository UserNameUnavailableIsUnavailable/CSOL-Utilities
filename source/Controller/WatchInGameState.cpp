#include "CController.hpp"
#include "CDateTime.hpp"
#include "CInGameState.hpp"
#include "CConsole.hpp"
#include "CEventList.hpp"
#include <Windows.h>
#include <__support/win32/locale_win32.h>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <libloaderapi.h>
#include <memory>

using namespace CSOL_Utilities;

CInGameState& CController::ResolveState()
{
    thread_local CInGameState in_game_state{ };
    thread_local auto log_path{ std::move(s_Instance->m_GameRootPath/L"Bin"/L"Error.log") };
    thread_local std::ifstream file_stream(log_path, std::ios::in | std::ios::ate); /* 创建线程时构造流，线程退出时流自动析构 */
    thread_local std::filesystem::file_time_type last_resolve_time{ };
    thread_local std::string line;
    thread_local std::size_t line_buf_capacity{ 64 };
    thread_local std::unique_ptr<char[]> line_buffer(new char[line_buf_capacity]);
    thread_local uint32_t return_to_room_count{ 0 };
    auto last_write_time = std::filesystem::last_write_time(log_path);
    bool updated{ last_write_time != last_resolve_time };
    const char* message = "";
    auto level{ CONSOLE_LOG_LEVEL::CLL_MESSAGE };
    auto state_literal{ IN_GAME_STATE::IGS_UNKNOWN };
    std::time_t state_time{ 0 };
    std::time_t current_time{ std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() };
    if (updated) {
        last_resolve_time = last_write_time;
        /* begin, end 初始都指向文件末尾，自后向前开始解析 */
        auto begin{ std::filesystem::file_size(log_path) };
        decltype(begin) end{ begin };
        while (begin > 0) {
            char c;
            file_stream.seekg(begin - 1); /* look forward */
            file_stream.get(c);
            /* now file_ptr = begin */
            if ((c == '\r' || c == '\n') && begin == end) {
                end = begin = begin - 1;
                continue;
            } else if (!(c == '\r' || c == '\n')) {
                begin--;
                continue;
            } else if ((c == '\r' || c == '\n') && begin != end) {
                if (line_buf_capacity < end - begin) {
                    line_buf_capacity = 2 * (end - begin);
                    line_buffer = std::move(std::unique_ptr<char[]>(new char[line_buf_capacity]));
                }
                file_stream.read(line_buffer.get(), end - begin);
                begin--;
                end = begin;
            }
            line = std::move(std::string(line_buffer.get()));
            for (auto& c : line) {
                c = std::toupper(c);
            }
            if (line.find("S_ROOM_ENTER") != std::string::npos) {
                state_literal = IN_GAME_STATE::IGS_IN_ROOM_NORMAL;
                message = "进入游戏房间";
                return_to_room_count = 0;
                break;
            } else if (line.find("JOIN HOSTSERVER") != std::string::npos) {
                state_literal = IN_GAME_STATE::IGS_LOADING;
                message = "加载游戏场景，将窗口暂时最小化";
                ShowWindow(s_Instance->m_hGameWindow, SW_MINIMIZE);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return_to_room_count = 0;
                break;
            } else if (line.find("RESULT CONFIRM") != std::string::npos) {
                state_literal = IN_GAME_STATE::IGS_IN_ROOM_NORMAL;
                message = "游戏结算完毕";
                return_to_room_count = 0;
                break;
            } else if (line.find("LEAVE GAME ROOM") != std::string::npos) {
                state_literal = IN_GAME_STATE::IGS_IN_HALL;
                message = "离开房间返回到游戏大厅";
                return_to_room_count = 0;
                break;
            } else if (line.find("RETURN TO ROOM") != std::string::npos) {
                state_literal = IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL;
                message = "异常返回到游戏房间";
                return_to_room_count++;
                break;
            } else if (line.find("GAME SHUTDOWN") != std::string::npos) {
                state_literal = IN_GAME_STATE::IGS_SHUTDOWN;
                message = "游戏客户端退出";
                return_to_room_count = 0;
                break;
            } else if (line.find("GAME SERVER LOGIN SUCCESS") != std::string::npos) {
                state_literal = IN_GAME_STATE::IGS_IN_HALL;
                message = "成功登录游戏客户端";
                return_to_room_count = 0;
                break;
            } else if (line.find("LOG FILE OPENED") != std::string::npos) {
                state_literal = IN_GAME_STATE::IGS_UNKNOWN;
                message = "等待游戏客户端加载";
                return_to_room_count = 0;
                break;
            } else {
                continue;
            }
        }
    }
    else
    {
        if (
            in_game_state.GetState() == IN_GAME_STATE::IGS_LOADING &&
            current_time - in_game_state.GetTimestamp() > 10 &&
            IsWindow(s_Instance->m_hGameWindow) &&
            !(WS_MINIMIZE & GetWindowLongW(s_Instance->m_hGameWindow, GWL_STYLE))
        ) { /* 游戏窗口加载完毕将刷新并恢复显示 */
            message = "游戏场景加载完成";
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            state_literal = IN_GAME_STATE::IGS_IN_MAP;
        } else if (
            in_game_state.GetState() == IN_GAME_STATE::IGS_LOADING &&
            current_time - in_game_state.GetTimestamp() > 40
        ) { /* 40 秒为场景加载的超时时间 */
            message = "游戏场景加载超过预设最长时间";
            level = CONSOLE_LOG_LEVEL::CLL_WARNING; /* 加载时间过长，发出警告 */
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            state_literal = IN_GAME_STATE::IGS_IN_MAP;
        } else if (
            in_game_state.GetState() == IN_GAME_STATE::IGS_LOGIN &&
            current_time - in_game_state.GetTimestamp() > 30
        ) { /* 登陆后，等待半分钟，以使窗口完全加载 */
            message = "登陆成功后等待时间达到 30 秒，去除游戏窗口标题栏并将其置顶";
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto module = LoadLibraryW(L".\\GamingTool.dll");
            auto MakeWindowBorderless = reinterpret_cast<void(*)(HWND)>(GetProcAddress(module, "MakeWindowBorderless"));
            if (MakeWindowBorderless) {
                MakeWindowBorderless(s_Instance->m_hGameWindow);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            state_literal = IN_GAME_STATE::IGS_IN_HALL;
        } else if (
            in_game_state.GetState() == IN_GAME_STATE::IGS_IN_ROOM_NORMAL &&
            current_time - in_game_state.GetTimestamp() > 15 * 60
        ) { /* 在房间内等待 15 分钟仍未开始游戏 */
            message = "在房间内等待时间已经超过 15 分钟，等待时间过长的房间将自动关闭";
            level = CONSOLE_LOG_LEVEL::CLL_WARNING;
            state_literal = IN_GAME_STATE::IGS_IN_HALL;
        } else if (
            in_game_state.GetState() == IN_GAME_STATE::IGS_IN_ROOM_NORMAL &&
            return_to_room_count > 3
        ) { /* 因异常原因返回到房间次数过多 */
            message = "因异常原因返回到房间内的次数过多（如：重连 1 2 3 问题、网络波动），此房间可能无法正常游戏";
            level = CONSOLE_LOG_LEVEL::CLL_WARNING;
            state_literal = IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL;
            return_to_room_count = 0;
        } else if (
            in_game_state.GetState() == IN_GAME_STATE::IGS_IN_MAP &&
            current_time - in_game_state.GetTimestamp() > 30 * 60
        ) {
            message = "长时间未能进行结算确认";
            level = CONSOLE_LOG_LEVEL::CLL_WARNING;
            state_literal = IN_GAME_STATE::IGS_IN_HALL;
        } else {
            return in_game_state;
        }
    }
    std::time_t file_time(std::chrono::duration_cast<std::chrono::seconds>(last_resolve_time.time_since_epoch()).count());
    std::time_t utc_midnight_time = file_time / (24 * 60 * 60) * (24 * 60 * 60); /* 当日午夜时间 */
    state_time = updated ? 
    CDateTime::ResolveMessageTimestamp(line, utc_midnight_time, nullptr, CDateTime::GetTimeBias()) :
    current_time;
    if (updated) CConsole::Log(CLL_MESSAGE, "%lld", state_time);
    if (in_game_state.update(state_literal, state_time).IsLastUpdateSuccessful()) {
        std::tm state_tm;
        std::tm current_tm;
        char state_time_string[32];
        char current_time_string[32];
        localtime_s(&current_tm, &current_time);
        localtime_s(&state_tm, &state_time);
        strftime(state_time_string, sizeof(state_time_string), "%Y/%m/%d %H:%M:%S", &state_tm);
        strftime(current_time_string, sizeof(current_time_string), "%Y/%m/%d %H:%M:%S", &current_tm);
        CConsole::Log(level,"当前时间：%lld（%d/%d/%d %d:%d:%d），状态迁移时间：%lld（%d/%d/%d %d:%d:%d）。内容：%s。",
        current_time, current_tm.tm_year + 1900, current_tm.tm_mon + 1, current_tm.tm_mday, current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec,
        state_time, state_tm.tm_year + 1900, state_tm.tm_mon + 1, state_tm.tm_mday, state_tm.tm_hour, state_tm.tm_min, state_tm.tm_sec,
        message);
    }
    return in_game_state;
}

void CController::DispatchCommand(const CInGameState& in_game_state) noexcept
{
    std::time_t current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    COMMAND cmd = COMMAND::CMD_NOP;
    if (in_game_state.GetState() == IN_GAME_STATE::IGS_LOADING) {
    } else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_MAP) {
        if (current_time - in_game_state.GetTimestamp() < 5) {
            cmd = COMMAND::CMD_CHOOSE_CLASS;
        } else if (current_time - in_game_state.GetTimestamp() < 120) {
            cmd = COMMAND::CMD_PLAY_GAME_NORMAL;
        } else {
            cmd = s_Instance->m_bExtendedAutoPlayMode ? COMMAND::CMD_PLAY_GAME_EXTEND : COMMAND::CMD_PLAY_GAME_NORMAL;
        }
    } else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_ROOM_NORMAL) {
        cmd = COMMAND::CMD_START_GAME_ROOM;
    } else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL) {
        cmd = COMMAND::CMD_CREATE_GAME_ROOM;
    } else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_HALL) {
        cmd = COMMAND::CMD_CREATE_GAME_ROOM;
    }
    s_Instance->m_Messenger.assign(cmd, current_time).dispatch();
}

void CController::WatchInGameState() noexcept
{
    try {
        CMannualEventList event_list{ &s_Instance->m_InGameStateWatcherSwitch, &s_Instance->m_GameProcessAlive };
        while (!s_Instance->m_bExitThreads) {
            event_list.WaitAll();
            s_Instance->m_InGameStateWatcherFinished.Reset(); /* 一定是在等待返回后才开始执行，并报告 */
            DispatchCommand(ResolveState());
            s_Instance->m_InGameStateWatcherFinished.Set();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    } catch (const std::exception& e) {
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, e.what());
    }
    CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 m_InGameStateWatcher 退出。");
    s_Instance->m_bThreadExit.Set();
}