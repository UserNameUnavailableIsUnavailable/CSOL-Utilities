#include "CConsole.hpp"
#include "CController.hpp"
#include "CDateTime.hpp"
#include "CEventList.hpp"
#include "CInGameState.hpp"
#include "CSOL_Utilities.hpp"
#include <Windows.h>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <libloaderapi.h>
#include <memory>
#include <thread>
#include <utility>

using namespace CSOL_Utilities;

CInGameState &CController::ResolveState()
{
    thread_local CInGameState in_game_state{};
    thread_local auto log_path{std::move(s_Instance->m_GameRootPath / L"Bin" / L"Error.log")};
    thread_local std::ifstream file_stream(log_path,
                                           std::ios::in | std::ios::ate); /* 创建线程时构造流，线程退出时流自动析构 */
    thread_local std::size_t line_buf_capacity{64};
    thread_local std::unique_ptr<char[]> line_buffer(new char[line_buf_capacity]);
    thread_local uint32_t return_to_room_count{0};
    thread_local auto last_resolve_file_time =
        std::filesystem::file_time_type{}; /* 最近一次解析时获取到的文件修改时间 */
    thread_local auto force_update{false};
    std::string new_line;

    auto current_file_time = std::filesystem::last_write_time(log_path); /* 当前文件修改时间 */
    bool file_modified{current_file_time != last_resolve_file_time};
    const char *message = "";
    auto level{CONSOLE_LOG_LEVEL::CLL_MESSAGE};
    auto state_literal{IN_GAME_STATE::IGS_UNKNOWN};
    std::time_t state_unix_time{0};
    std::time_t current_unix_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    thread_local std::string line;
    /* 文件修改或强制更新触发对文件的读取 */
    if (file_modified || force_update)
    {
        /* begin, end 初始都指向文件末尾，自后向前开始解析 */
        last_resolve_file_time = current_file_time;
        force_update = false;
        auto begin{std::filesystem::file_size(log_path)};
        auto end{begin};
        std::time_t file_unix_time(CDateTime::FileTimeToUnixTime(current_file_time));
        std::time_t utc_midnight_unix_time = file_unix_time / (24 * 60 * 60) * (24 * 60 * 60); /* 当日午夜时间 */
        while (begin > std::ios::pos_type(0))
        {
            /* 倒序按行读取文件，解析得到 new_line */
            if (!file_stream.seekg(begin - std::ios::off_type(1), std::ios::beg))
            {
                file_stream.clear();
                std::this_thread::sleep_for(std::chrono::seconds(3));
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_WARNING, "文件操作失败，将在一段时间后重试。");
                break;
            }
            char c = file_stream.get();
            /* now file_ptr = begin */
            if ((c == '\r' || c == '\n') && begin == end)
            {
                end = begin = begin - std::ios::off_type(1);
                continue;
            }
            else if (!(c == '\r' || c == '\n'))
            {
                begin -= std::ios::off_type(1);
                continue;
            }
            else if ((c == '\r' || c == '\n') && begin != end)
            {
                if (line_buf_capacity < end - begin)
                {
                    line_buf_capacity = 2 * (end - begin);
                    line_buffer = std::unique_ptr<char[]>(new char[line_buf_capacity]);
                }
                file_stream.read(line_buffer.get(), end - begin);
                line_buffer[end - begin] = 0;
                end = begin = begin - std::ios::off_type(1);
            }
            new_line = std::string(line_buffer.get());
            /* 将字符全部转为大写 */
            for (auto &c : new_line)
            {
                c = std::toupper(c);
            }
            if (new_line == line)
            {
                break;
            }
            // CConsole::Log(CLL_MESSAGE, new_line);
            /* 解析新得到的串 */
            if (new_line.find("S_ROOM_ENTER") != std::string::npos)
            {
                message = "进入游戏房间";
                return_to_room_count = 0;
                state_literal = IN_GAME_STATE::IGS_IN_ROOM_NORMAL;
                state_unix_time = CDateTime::ResolveMessageTimestamp(new_line, utc_midnight_unix_time, nullptr,
                                                                     CDateTime::GetTimeBias());
                line = std::move(new_line);
                break;
            }
            else if (new_line.find("JOIN HOSTSERVER") != std::string::npos)
            {
                message = "加载游戏场景，将窗口暂时最小化";
                ShowWindow(s_Instance->m_hGameWindow, SW_MINIMIZE);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return_to_room_count = 0;
                state_literal = IN_GAME_STATE::IGS_LOADING;
                state_unix_time = CDateTime::ResolveMessageTimestamp(new_line, utc_midnight_unix_time, nullptr,
                                                                     CDateTime::GetTimeBias());
                line = std::move(new_line);
                break;
            }
            else if (new_line.find("RESULT CONFIRM") != std::string::npos)
            {
                message = "游戏结算完毕";
                return_to_room_count = 0;
                state_literal = IN_GAME_STATE::IGS_IN_ROOM_NORMAL;
                state_unix_time = CDateTime::ResolveMessageTimestamp(new_line, utc_midnight_unix_time, nullptr,
                                                                     CDateTime::GetTimeBias());
                line = std::move(new_line);
                break;
            }
            else if (new_line.find("LEAVE GAME ROOM") != std::string::npos)
            {
                message = "离开房间返回到游戏大厅";
                return_to_room_count = 0;
                state_literal = IN_GAME_STATE::IGS_IN_HALL;
                state_unix_time = CDateTime::ResolveMessageTimestamp(new_line, utc_midnight_unix_time, nullptr,
                                                                     CDateTime::GetTimeBias());
                line = std::move(new_line);
                break;
            }
            else if (new_line.find("RETURN TO ROOM") != std::string::npos)
            {
                message = "异常返回到游戏房间";
                return_to_room_count++;
                state_literal = IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL;
                state_unix_time = CDateTime::ResolveMessageTimestamp(new_line, utc_midnight_unix_time, nullptr,
                                                                     CDateTime::GetTimeBias());
                line = std::move(new_line);
                break;
            }
            else if (new_line.find("GAME SHUTDOWN") != std::string::npos)
            {
                message = "游戏客户端退出";
                return_to_room_count = 0;
                state_literal = IN_GAME_STATE::IGS_SHUTDOWN;
                state_unix_time = CDateTime::ResolveMessageTimestamp(new_line, utc_midnight_unix_time, nullptr,
                                                                     CDateTime::GetTimeBias());
                line = std::move(new_line);
                break;
            }
            else if (new_line.find("GAME SERVER LOGIN SUCCESS") != std::string::npos)
            {
                message = "成功登录游戏客户端";
                return_to_room_count = 0;
                state_literal = IN_GAME_STATE::IGS_LOGIN;
                state_unix_time = CDateTime::ResolveMessageTimestamp(new_line, utc_midnight_unix_time, nullptr,
                                                                     CDateTime::GetTimeBias());
                line = std::move(new_line);
                break;
            }
            else if (new_line.find("LOG FILE OPENED") != std::string::npos)
            {
                message = "状态未知";
                level = CONSOLE_LOG_LEVEL::CLL_WARNING;
                return_to_room_count = 0;
                state_literal = IN_GAME_STATE::IGS_UNKNOWN;
                state_unix_time = CDateTime::ResolveMessageTimestamp(new_line, utc_midnight_unix_time, nullptr,
                                                                     CDateTime::GetTimeBias());
                line = std::move(new_line);
                break;
            }
            else
            {
                /* 未能获取到状态 */
                continue;
            }
        }
    }
    /* 例行检查 */
    else
    {
        if (in_game_state.GetState() == IN_GAME_STATE::IGS_LOADING &&
            current_unix_time - in_game_state.GetTimestamp() > 10 && IsWindow(s_Instance->m_hGameWindow) &&
            !(WS_MINIMIZE & GetWindowLongW(s_Instance->m_hGameWindow, GWL_STYLE)))
        { /* 游戏窗口加载完毕将刷新并恢复显示 */
            message = "游戏场景加载完成";
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            SetWindowPos(s_Instance->m_hGameWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            state_literal = IN_GAME_STATE::IGS_IN_MAP;
            state_unix_time = current_unix_time;
        }
        else if (in_game_state.GetState() == IN_GAME_STATE::IGS_LOADING &&
                 current_unix_time - in_game_state.GetTimestamp() > 40)
        { /* 40 秒为场景加载的超时时间 */
            message = "游戏场景加载超过预设最长时间";
            level = CONSOLE_LOG_LEVEL::CLL_WARNING; /* 加载时间过长，发出警告 */
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            SetWindowPos(s_Instance->m_hGameWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            state_literal = IN_GAME_STATE::IGS_IN_MAP;
            state_unix_time = current_unix_time;
        }
        else if (in_game_state.GetState() == IN_GAME_STATE::IGS_LOGIN &&
                 current_unix_time - in_game_state.GetTimestamp() > 30)
        { /* 登陆后，等待半分钟，以使窗口完全加载 */
            message = "登陆成功后等待时间达到 30 秒";
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            SetWindowPos(s_Instance->m_hGameWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            auto MakeWindowBorderless =
                reinterpret_cast<void (*)(HWND)>(GetProcAddress(s_Instance->m_hDllMod, "MakeWindowBorderless"));
            if (MakeWindowBorderless)
            {
                MakeWindowBorderless(s_Instance->m_hGameWindow);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            state_literal = IN_GAME_STATE::IGS_IN_HALL;
            state_unix_time = current_unix_time;
        }
        else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_ROOM_NORMAL &&
                 current_unix_time - in_game_state.GetTimestamp() > s_Instance->m_MaxWaitTimeInGameRoom)
        { /* 在房间内等待超时而仍未开始游戏 */
            message = "在房间内等待时间已经超过预设时间，等待时间过长的房间将自动关闭";
            level = CONSOLE_LOG_LEVEL::CLL_WARNING;
            state_literal = IN_GAME_STATE::IGS_IN_HALL;
            state_unix_time = current_unix_time;
        }
        else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_ROOM_NORMAL && return_to_room_count > 3)
        { /* 因异常原因返回到房间次数过多 */
            message = "因异常原因返回到房间内的次数过多（如：重连 1 2 3 问题、网络波动），此房间可能无法正常游戏";
            level = CONSOLE_LOG_LEVEL::CLL_WARNING;
            return_to_room_count = 0;
            state_literal = IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL;
            state_unix_time = current_unix_time;
        }
        else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_MAP &&
                 current_unix_time - in_game_state.GetTimestamp() > 30 * 60)
        {
            message = "长时间未能进行结算确认";
            level = CONSOLE_LOG_LEVEL::CLL_WARNING;
            state_literal = IN_GAME_STATE::IGS_IN_HALL;
            state_unix_time = current_unix_time;
        }
        else if (in_game_state.GetState() == IN_GAME_STATE::IGS_UNKNOWN &&
                 current_unix_time - in_game_state.GetTimestamp() > 10)
        {
            force_update = true;
            message = "再次尝试确定状态";
            state_unix_time = current_unix_time;
        }
    }
    if (in_game_state.update(state_literal, state_unix_time).IsLastUpdateSuccessful())
    {
        std::tm state_tm;
        char state_time_string[32];
        localtime_s(&state_tm, &state_unix_time);
        strftime(state_time_string, sizeof(state_time_string), "%Y/%m/%d %H:%M:%S", &state_tm);
        CConsole::Log(level, "状态机时间：%s。内容：%s。", state_time_string, message);
    }
    return in_game_state;
}

void CController::DispatchCommand(const CInGameState &in_game_state) noexcept
{
    std::time_t current_time =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    COMMAND cmd = COMMAND::CMD_NOP;
    if (in_game_state.GetState() == IN_GAME_STATE::IGS_LOADING)
    {
        cmd = COMMAND::CMD_NOP;
    }
    else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_MAP)
    {
        if (current_time - in_game_state.GetTimestamp() < 5)
        {
            cmd = COMMAND::CMD_CHOOSE_CLASS;
        }
        else if (current_time - in_game_state.GetTimestamp() < 120)
        {
            cmd = COMMAND::CMD_PLAY_GAME_NORMAL;
        }
        else
        {
            cmd = s_Instance->m_ExtendedAutoPlayMode ? COMMAND::CMD_PLAY_GAME_EXTEND : COMMAND::CMD_PLAY_GAME_NORMAL;
        }
    }
    else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_ROOM_NORMAL)
    {
        cmd = COMMAND::CMD_START_GAME_ROOM;
    }
    else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL)
    {
        cmd = COMMAND::CMD_CREATE_GAME_ROOM;
    }
    else if (in_game_state.GetState() == IN_GAME_STATE::IGS_IN_HALL)
    {
        cmd = COMMAND::CMD_CREATE_GAME_ROOM;
    }
    s_Instance->m_Messenger.AssignAndDispatch(cmd, current_time);
}

void CController::WatchInGameState() noexcept
{
    try
    {
        CMannualEventList event_list{
            &s_Instance->m_InGameStateWatcherSwitch,
            &s_Instance->m_GameProcessAlive,
        };
        while (true)
        {
            event_list.WaitAll();
            if (s_Instance->m_ExitThreads)
            {
                break;
            }
            s_Instance->m_InGameStateWatcherFinished.Reset(); /* 一定是在等待返回后才开始执行，并报告 */
            DispatchCommand(ResolveState());
            s_Instance->m_InGameStateWatcherFinished.Set();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    catch (const std::exception &e)
    {
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, e.what());
    }
    CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 m_InGameStateWatcher 退出。");
    s_Instance->m_ThreadExitEvent.Set();
}