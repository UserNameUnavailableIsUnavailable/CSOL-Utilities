#include "CConsole.hpp"
#include "CController.hpp"
#include "CDateTime.hpp"
#include "CEventList.hpp"
#include "CInGameState.hpp"
#include "CSOL_Utilities.hpp"
#include "CException.hpp"
#include <Windows.h>
#include <cctype>
#include <chrono>
#include <clocale>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <errhandlingapi.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <handleapi.h>
#include <libloaderapi.h>
#include <memory>
#include <cassert>
#include <synchapi.h>
#include <thread>
#include <type_traits>
#include <utility>
#include <winnt.h>

using namespace CSOL_Utilities;

bool CController::AnalyzeInGameState(std::string str, std::time_t unix_time)
{
    using UniqueHandle = std::unique_ptr<typename std::remove_pointer<HANDLE>::type, decltype(&CloseHandle)>;
    thread_local UniqueHandle hTimer(CreateWaitableTimerW(nullptr, true, nullptr), &CloseHandle);
    if (!hTimer)
    {
        throw CException("创建定时器失败，错误代码：%lu。", GetLastError());
    }
    thread_local std::string last_str; /* 上一次传入的字符串副本 */
    for (auto& c : str)
    {
        c = std::toupper(c); /* 使用默认的 "C" 区域格式转换，仅转换 ASCII 字符 */
    }
    /* 字符串完全相同，说明与上次状态完全一致，不进行任何改动，原样返回上次的分析结果 */
    if (str == last_str)
    {
        return false;
    }

    const char* message = nullptr;
    thread_local int return_to_room_count = 0; /* 返回到游戏房间的次数 */
    CONSOLE_LOG_LEVEL level = CONSOLE_LOG_LEVEL::CLL_MESSAGE;

    /* 对字符串进行分析 */
    if (str.find("S_ROOM_ENTER") != std::string::npos)
    {
        s_Instance->m_InGameState.update(IN_GAME_STATE::IGS_IN_ROOM_NORMAL, unix_time);
        message = "进入游戏房间";
    }
    else if (str.find("JOIN HOSTSERVER") != std::string::npos)
    {
        ShowWindow(s_Instance->m_hGameWindow, SW_MINIMIZE);
        s_Instance->m_InGameState.update(IN_GAME_STATE::IGS_LOADING, unix_time);
        message = "等待游戏场景加载，将游戏窗口暂时最小化";
        LARGE_INTEGER li = { .QuadPart = 40 * 1000 * 1000 * 10 }; /* 度量单位为 100 ns */
        SetWaitableTimer(hTimer.get(), &li, 0, [] (void*, DWORD, DWORD) {
            /* 窗口有效，且尚未恢复 */
            if (!IsWindow(s_Instance->m_hGameWindow) || !(WS_MINIMIZE & GetWindowLongW(s_Instance->m_hGameWindow, GWL_STYLE)))
            {
                return;
            }
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            SetWindowPos(s_Instance->m_hGameWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            CConsole::Log(CONSOLE_LOG_LEVEL::CLL_WARNING, "游戏加载超时时间到，恢复游戏窗口显示。");
        }, nullptr, false);
    }
    else if (str.find("RESULT CONFIRM") != std::string::npos)
    {
        s_Instance->m_InGameState.update(IN_GAME_STATE::IGS_IN_ROOM_NORMAL, unix_time);
        message = "游戏结算完成";
    }
    else if (str.find("LEAVE GAME ROOM") != std::string::npos)
    {
        s_Instance->m_InGameState.update(IN_GAME_STATE::IGS_IN_HALL, unix_time);
        message = "离开房间，返回到游戏大厅";
        level = CONSOLE_LOG_LEVEL::CLL_WARNING;
    }
    else if (str.find("RETURN TO ROOM") != std::string::npos)
    {
        s_Instance->m_InGameState.update(IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL, unix_time);
        return_to_room_count++;
        message = "因网络问题返回到游戏房间";
        level = CONSOLE_LOG_LEVEL::CLL_WARNING;
    }
    else if (str.find("GAME SHUTDOWN") != std::string::npos)
    {
        s_Instance->m_InGameState.update(IN_GAME_STATE::IGS_SHUTDOWN, unix_time);
        return_to_room_count = 0;
        message = "游戏客户端退出";
        level = CONSOLE_LOG_LEVEL::CLL_WARNING;
    }
    else if (str.find("GAME SERVER LOGIN SUCCESS") != std::string::npos)
    {
        s_Instance->m_InGameState.update(IN_GAME_STATE::IGS_LOGIN, unix_time);
        return_to_room_count = 0;
        message = "成功登录游戏客户端，等待客户端界面完全加载";
    }
    else if (str.find("LOG FILE OPENED") != std::string::npos)
    {
        s_Instance->m_InGameState.update(IN_GAME_STATE::IGS_UNKNOWN, unix_time);
        return_to_room_count = 0;
        message = "未知状态";
        level = CONSOLE_LOG_LEVEL::CLL_WARNING;
    }
    /* 未发生更新 */
    else
    {
        return false;
    }
    /* 发生更新 */
    last_str = std::move(str);
    return true;
}

const CInGameState &CController::ResolveState()
{
    thread_local CInGameState in_game_state{}; /* 游戏内状态 */
    thread_local auto log_path{std::move(s_Instance->m_GameRootPath / L"Bin" / L"Error.log")}; /* 日志文件路径 */
    thread_local std::ifstream file_stream(log_path,
                                           std::ios::in | std::ios::ate); /* 创建线程时构造文件流，线程退出时流自动析构 */
    thread_local uint32_t return_to_room_count{ 0 }; /* 异常返回到房间计数器 */
    thread_local auto force_read{ false }; /* 强制读取标志 */
    thread_local std::vector<char> reversed_file_content; /* 倒序读取到的文件内容 */
    std::string line;
    const char* message = "";
    auto level{ CONSOLE_LOG_LEVEL::CLL_MESSAGE }; /* 日志级别：消息、警告、错误 */
    auto state_literal{ IN_GAME_STATE::IGS_UNKNOWN }; /* 当前游戏内状态 */
    std::time_t state_unix_time{ 0 }; /* 状态的 UNIX 时间戳 */
    std::time_t current_unix_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); /* 当前系统时间的 UNIX 时间戳表示 */
    /* 文件修改或强制更新触发对文件的读取 */
    thread_local auto last_resolve_file_time =
        std::filesystem::file_time_type{}; /* 最近一次解析时获取到的文件修改时间 */
    auto current_file_time = std::filesystem::last_write_time(log_path); /* 当前文件修改时间 */
    bool file_modified{ current_file_time != last_resolve_file_time }; /* 根据最近一次解析时间和文件最近一次写入时间判断文件是否发生更新 */
    if (file_modified || force_read)
    {
        last_resolve_file_time = current_file_time;
        if (force_read)
        {
            s_Instance->m_InGameState.reset();
            force_read = false;
        }
        /* begin, end 初始都指向文件末尾，自后向前开始解析 */
        auto begin{std::filesystem::file_size(log_path)};
        auto end{ begin };
        std::time_t file_unix_time(CDateTime::FileTimeToUnixTime(current_file_time));
        std::time_t utc_midnight_unix_time = file_unix_time / (24 * 60 * 60) * (24 * 60 * 60); /* 当日午夜时间 */
        /* 倒序对文件进行解析 */
        while (begin > static_cast<std::ios::pos_type>(0))
        {
            thread_local auto io_error_count{ 0 }; /* 读取文件错误计数器 */
            char c;
            /* 倒序按行读取文件，解析得到 new_line */
            if (!file_stream.seekg(begin - std::ios::off_type(1), std::ios::beg) || !file_stream.get(c)) /* lookahead(1)，倒序向前获取一个字符 */
            {
                io_error_count++;
                if (io_error_count == 9) /* 读取错误次数超过 9 次，认为无法正常运行 */
                {
                    throw CException("文件读取错误次数过多，当前线程无法正常运行。");
                }
                file_stream.clear(); /* clear error flags on error */
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_WARNING, "文件读取操作失败，将在一段时间后重试。");
                std::this_thread::sleep_for(std::chrono::seconds(3));
                break; /* break 后状态不发生任何更新 */
            }
            /* 读取成功 */
            io_error_count = 0;
            if ((c == '\r' || c == '\n') && begin == end)
            {
                end = begin = begin - static_cast<std::ios::off_type>(1);
                continue;
            }
            else if (!(c == '\r' || c == '\n'))
            {
                begin -= static_cast<std::ios::off_type>(1);
                reversed_file_content.push_back(c);
                continue;
            }
            else if ((c == '\r' || c == '\n') && begin != end)
            {
                line = std::string(reversed_file_content.rbegin(), reversed_file_content.rend());
                reversed_file_content.clear(); /* set size = 0 */
                end = begin = begin - static_cast<std::ios::off_type>(1);
            }
            if (AnalyzeInGameState(line, file_unix_time))
            {
                break;
            }
        //     /* 将字符全部转为大写 */
        //     for (auto &c : line)
        //     {
        //         c = std::toupper(c);
        //     }
        //     if (line.find("S_ROOM_ENTER") != std::string::npos)
        //     {
        //         message = "进入游戏房间";
        //         return_to_room_count = 0;
        //         state_literal = IN_GAME_STATE::IGS_IN_ROOM_NORMAL;
        //         state_unix_time = file_unix_time;
        //         break;
        //     }
        //     else if (line.find("JOIN HOSTSERVER") != std::string::npos)
        //     {
        //         ShowWindow(s_Instance->m_hGameWindow, SW_MINIMIZE);
        //         std::this_thread::sleep_for(std::chrono::seconds(1));
        //         message = "加载游戏场景，将窗口暂时最小化";
        //         return_to_room_count = 0;
        //         state_literal = IN_GAME_STATE::IGS_LOADING;
        //         state_unix_time = file_unix_time;
        //         break;
        //     }
        //     else if (line.find("RESULT CONFIRM") != std::string::npos)
        //     {
        //         ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
        //         std::this_thread::sleep_for(std::chrono::seconds(1));
        //         message = "游戏结算完毕";
        //         return_to_room_count = 0;
        //         state_literal = IN_GAME_STATE::IGS_IN_ROOM_NORMAL;
        //         state_unix_time = file_unix_time;
        //         break;
        //     }
        //     else if (line.find("LEAVE GAME ROOM") != std::string::npos)
        //     {
        //         message = "离开房间返回到游戏大厅";
        //         return_to_room_count = 0;
        //         state_literal = IN_GAME_STATE::IGS_IN_HALL;
        //         state_unix_time = file_unix_time;
        //         break;
        //     }
        //     else if (line.find("RETURN TO ROOM") != std::string::npos)
        //     {
        //         ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
        //         std::this_thread::sleep_for(std::chrono::seconds(1));
        //         message = "异常返回到游戏房间";
        //         return_to_room_count++;
        //         level = CONSOLE_LOG_LEVEL::CLL_WARNING;
        //         state_literal = IN_GAME_STATE::IGS_IN_ROOM_ABNORMAL;
        //         state_unix_time = file_unix_time;
        //         break;
        //     }
        //     else if (line.find("GAME SHUTDOWN") != std::string::npos)
        //     {
        //         message = "游戏客户端退出";
        //         level = CONSOLE_LOG_LEVEL::CLL_WARNING;
        //         return_to_room_count = 0;
        //         state_literal = IN_GAME_STATE::IGS_SHUTDOWN;
        //         state_unix_time = file_unix_time;
        //         break;
        //     }
        //     else if (line.find("GAME SERVER LOGIN SUCCESS") != std::string::npos)
        //     {
        //         message = "成功登录游戏客户端";
        //         return_to_room_count = 0;
        //         state_literal = IN_GAME_STATE::IGS_LOGIN;
        //         state_unix_time = file_unix_time;
        //         break;
        //     }
        //     else if (line.find("LOG FILE OPENED") != std::string::npos)
        //     {
        //         message = "状态未知";
        //         level = CONSOLE_LOG_LEVEL::CLL_WARNING;
        //         return_to_room_count = 0;
        //         state_literal = IN_GAME_STATE::IGS_UNKNOWN;
        //         state_unix_time = file_unix_time;
        //         break;
        //     }
        //     else
        //     {
        //         /* 未能获取到状态 */
        //         continue;
        //     }
        }

    }
    /* 例行检查 */
    else
    {
        if (in_game_state.GetState() == IN_GAME_STATE::IGS_LOADING &&
            current_unix_time - in_game_state.GetTimestamp() > 10 && IsWindow(s_Instance->m_hGameWindow) &&
            !(WS_MINIMIZE & GetWindowLongW(s_Instance->m_hGameWindow, GWL_STYLE)))
        { /* 游戏窗口加载完毕将刷新并恢复显示 */
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            message = "游戏场景加载完成";
            SetWindowPos(s_Instance->m_hGameWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            state_literal = IN_GAME_STATE::IGS_IN_MAP;
            state_unix_time = current_unix_time;
        }
        else if (in_game_state.GetState() == IN_GAME_STATE::IGS_LOADING &&
                 current_unix_time - in_game_state.GetTimestamp() > 40)
        { /* 40 秒为场景加载的超时时间 */
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            message = "游戏场景加载超过预设最长时间";
            level = CONSOLE_LOG_LEVEL::CLL_WARNING; /* 加载时间过长，发出警告 */
            SetWindowPos(s_Instance->m_hGameWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            state_literal = IN_GAME_STATE::IGS_IN_MAP;
            state_unix_time = current_unix_time;
        }
        else if (in_game_state.GetState() == IN_GAME_STATE::IGS_LOGIN &&
                 current_unix_time - in_game_state.GetTimestamp() > 30)
        { /* 登陆后，等待半分钟，以使窗口完全加载 */
            ShowWindow(s_Instance->m_hGameWindow, SW_NORMAL);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            message = "登陆成功后等待时间达到 30 秒";
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
            force_read = true;
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
        MSG msg;
        while (true)
        {
            event_list.WaitAll();
            if (s_Instance->m_ExitThreads)
            {
                break;
            }
            s_Instance->m_InGameStateWatcherFinished.Reset(); /* 一定是在等待返回后才开始执行，并报告 */
            DispatchCommand(ResolveState());
            MSG msg;
            s_Instance->m_InGameStateWatcherFinished.Set();
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        };
    }
    catch (const std::exception &e)
    {
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, e.what());
    }
    CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 m_InGameStateWatcher 退出。");
    s_Instance->m_ThreadExitEvent.Set();
}