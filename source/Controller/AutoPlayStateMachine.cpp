#include "EventList.hpp"
#include "Console.hpp"
#include "InGameState.hpp"
#include "CSOL_Utilities.hpp"
#include "OCR/OcrLiteCApi.h"
#include "Controller.hpp"
#undef max
#undef min
#include "aho_corasick/aho_corasick.hpp"
#include <cassert>
#include <chrono>
#include <ctime>
#include <memory>
#include <array>
#include <climits>
#include <WindowCapture.hpp>
#include <Exception.hpp>

// #define _RECOGNITION_RESULT

namespace CSOL_Utilities
{
constexpr const char* GAME_IMAGE_FILE_NAME_UTF8 = "$~capture.bmp";
constexpr const wchar_t* GAME_IMAGE_FILE_NAME_UTF16 = L"$~capture.bmp";
/* 关键字 */
/* 大厅中可能被检测到的文本 */
const char* HALL_TEXT[]{ "公告栏", "教程大纲", "通过好友", "返回", "新建房间", "进入房间", "输入房间号", "搜索" };
/* 等候房间内可能被检测到的文本 */
const char* ROOM_TEXT[] { "房间信息", "设置视角", "玩家列表", "添加技能插件", "技能插件目录", "邀请", "游戏难度说明", "屏蔽列表添加", "观战", "等待中", "游戏进行中", "自动添加", "详细设定" };
/* 游戏加载过程中可能被检测到的文本 */
const char* LOADING_TEXT[]{ "自定义游戏", "与服务器连接中", "认证游戏资源", "预缓存资源", "游戏信息读取", "最佳奖励", "正在下载" };
/* 游戏进行中可能被检测到的文本 */
const char* MAP_TEXT[]{ "录制", "录像", "视频", "取消", "选择角色", "剩余时间", "自动选择", "下一个", "延迟时间", "选择武器",
	"重新购买", "开启提示", "僵尸的战利品", "无法携带更多", "连续杀敌", "全员杀敌", "无法丢弃该武器", "通关失败",
    "使用回合重置道具", "购买菜单", "金币总计", "胜率", "成功通关", "通关时间", "已成功完成一局游戏", "胜率", "当前关数", "联赛积分", "武器喜好调查"
};
void Controller::AnalyzeInGameState()
{
    thread_local aho_corasick::trie trie;
    thread_local bool hasTrieInitialized{ false };
    if (!hasTrieInitialized)
    {
		for (auto i : HALL_TEXT)
		{
			trie.insert(i);
		}
		for (auto i : ROOM_TEXT)
		{
			trie.insert(i);
		}
		for (auto i : LOADING_TEXT)
		{
			trie.insert(i);
		}
		for (auto i : MAP_TEXT)
		{
			trie.insert(i);
		}
        hasTrieInitialized = true;
    }
    std::time_t current_time;
    InGameState& state = s_Instance->m_CurrentState;
    {
		std::lock_guard lk(s_Instance->m_GameInfoMutex);
        HWND hWnd = s_Instance->m_hGameWindow;
		if (!hWnd || !IsWindow(hWnd))
		{
			return;
		}
        if (IsIconic(hWnd))
        {
			ShowWindow(hWnd, SW_NORMAL);
        }
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//#ifdef _DEBUG
//        auto capture_start = std::chrono::system_clock::now();
//#endif
        /* 截取游戏界面图像 */
        thread_local WindowCapture wc;
        thread_local int successive_error_count = 0;
        try
        {
			UniqueHandle hFile = CreateFileW(GAME_IMAGE_FILE_NAME_UTF16, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (!hFile)
            {
                throw Exception("创建文件失败。错误代码：%lu。", GetLastError());
            }
			wc.Capture(hWnd).Save(hFile.get());
        }
        catch (Exception& e)
        {
            Console::Log(e.GetLevel(), e.what());
            successive_error_count++;
            if (successive_error_count > 9)
            {
                throw Exception("错误次数过多。");
            }
			return;
        }
        successive_error_count = 0; /* 本次未出现错误，则将错误计数置为 0 */
//#ifdef _DEBUG
//        auto capture_end = std::chrono::system_clock::now();
//        auto capture_elapse = std::chrono::duration_cast<std::chrono::milliseconds>(capture_end - capture_start);
//        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_DEBUG, "本次截图耗时：%llu。", capture_elapse);
//#endif // _DEBUG
    }
    thread_local OCR_PARAM param{ 0 };
    thread_local auto hOcr = s_Instance->m_hOcr;
    if (!std::filesystem::is_regular_file(GAME_IMAGE_FILE_NAME_UTF16))
    {
        return;
    }
#ifdef _DEBUG
    auto recognition_start = std::chrono::system_clock::now();
#endif
    OCR_BOOL bRet = OcrDetect(hOcr, "./", GAME_IMAGE_FILE_NAME_UTF8, &param);
#ifdef _DEBUG
    auto recognition_end = std::chrono::system_clock::now();
    auto recognition_elapse = std::chrono::duration_cast<std::chrono::milliseconds>(recognition_end - recognition_start);
    Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_DEBUG, "本次识别耗时：%llu", recognition_elapse);
#endif
    thread_local std::int32_t buffer_size{ 8192 };
    assert(buffer_size > 0);
    thread_local std::unique_ptr<char> pBuffer(new char[buffer_size]);
    if (!bRet)
    {
        return;
    }
    std::int64_t len = OcrGetLen(hOcr);
    if (len <= 0)
    {
        return;
    }
    if (len >= buffer_size)
    {
        buffer_size = len + len / 2; /* 扩大 1.5 倍 */
        pBuffer = std::unique_ptr<char>(new char[buffer_size]);
    }
    assert(buffer_size < INT_MAX);
    if (!OcrGetResult(hOcr, pBuffer.get(), buffer_size))
    {
        return;
    }
#if  defined(_RECOGNITION_RESULT) && defined (_DEBUG)
	printf("%s\r\n", pBuffer.get());
#endif // _RECOGNITION_RESULT
    /* 分析当前游戏内情况 */
#ifdef _DEBUG
    auto parse_start = std::chrono::system_clock::now();
#endif
    IN_GAME_STATE current_state_typename;
    auto result = trie.parse_text(pBuffer.get());
    if (result.empty())
    {
        current_state_typename = IN_GAME_STATE::IGS_UNKNOWN;
    }
    else
    {
        struct InGameStatistics
        {
            int occurrences = 0;
            IN_GAME_STATE state = IN_GAME_STATE::IGS_UNKNOWN;
            InGameStatistics& operator++() noexcept
            {
                ++occurrences;
                return *this;
            }
            InGameStatistics operator++(int) noexcept
            {
                auto tmp = *this;
                operator++();
                return tmp;
            }
        };
        InGameStatistics hall{ 0, IN_GAME_STATE::IGS_IN_HALL };
        InGameStatistics room{ 0, IN_GAME_STATE::IGS_IN_ROOM };
        InGameStatistics loading{ 0, IN_GAME_STATE::IGS_LOADING };
        InGameStatistics map{ 0, IN_GAME_STATE::IGS_IN_MAP };
		for (auto i = result.begin(); i != result.end(); i++)
		{
			if (i->get_index() < ARRAYSIZE(HALL_TEXT))
			{
                ++hall;
			}
			else if (i->get_index() < ARRAYSIZE(HALL_TEXT) + ARRAYSIZE(ROOM_TEXT))
			{
                ++room;
			}
			else if (i->get_index() < ARRAYSIZE(HALL_TEXT) + ARRAYSIZE(ROOM_TEXT) + ARRAYSIZE(LOADING_TEXT))
			{
                ++loading;
			}
			else
			{
                ++map;
			}
            std::array<InGameStatistics*, 4> list{ &hall, &room, &loading, &map };
            auto result = std::max_element(list.begin(), list.end(), [](InGameStatistics* a, InGameStatistics* b) {
                return a->occurrences < b->occurrences;
            });
            current_state_typename = (*result)->state;
		}
    }
#ifdef _DEBUG
    auto parse_end = std::chrono::system_clock::now();
    auto parse_elapse = std::chrono::duration_cast<std::chrono::milliseconds>(parse_end - parse_start);
    Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_DEBUG, "本次解析耗时：%llu", parse_elapse);
#endif
    thread_local int abnormal_back_to_room = 0;
    /* 合法状态迁移 */
    /* 手写状态机，考虑到后续维护难度，故引入了更多的代码冗余降低理解难度 */
    /* 5 种自迁移 */
    if (state.GetState() == current_state_typename)
    {

    }
    /* 5 种顺序迁移 */
    else if (state.GetState() == IN_GAME_STATE::IGS_LOGIN && current_state_typename == IN_GAME_STATE::IGS_IN_HALL) /* 登陆成功，进入大厅 */
    {
        state.update(current_state_typename, current_time);
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "登陆成功，进入大厅。");
    }
    else if (state.GetState() == IN_GAME_STATE::IGS_IN_HALL && current_state_typename == IN_GAME_STATE::IGS_IN_ROOM) /* 创建房间 */
    {
        state.update(current_state_typename, current_time);
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "进入游戏房间。");
    }
    else if (state.GetState() == IN_GAME_STATE::IGS_IN_ROOM && current_state_typename == IN_GAME_STATE::IGS_LOADING) /* 开始游戏 */
    {
        state.update(current_state_typename, current_time);
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "开始游戏，加载游戏场景。");
    }
    else if (state.GetState() == IN_GAME_STATE::IGS_LOADING && current_state_typename == IN_GAME_STATE::IGS_IN_MAP) /* 场景加载完成 */
    {
        state.update(current_state_typename, current_time);
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "游戏场景加载完成。");
    }
    else if (state.GetState() == IN_GAME_STATE::IGS_IN_MAP && current_state_typename == IN_GAME_STATE::IGS_IN_ROOM) /* 结算确认 */
    {
        state.update(current_state_typename, current_time);
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "结算确认，回到游戏房间。");
    }
    /* 消除未知状态迁移，不计入迁移种类 */
    else if (current_state_typename == IN_GAME_STATE::IGS_UNKNOWN)
    {
        if (state.GetState() != IN_GAME_STATE::IGS_IN_MAP && state.GetState() != IN_GAME_STATE::IGS_LOADING)
        {
			state.update(IN_GAME_STATE::IGS_UNKNOWN, current_time);
        }
        else
        {
            /* 正在游戏 */
        }
    }
    else if (state.GetState() == IN_GAME_STATE::IGS_UNKNOWN && current_state_typename != IN_GAME_STATE::IGS_UNKNOWN)
    {
        state.update(current_state_typename, current_time);
        switch (current_state_typename)
        {
        case IN_GAME_STATE::IGS_IN_HALL: Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "当前场景确定为游戏大厅。"); break;
        case IN_GAME_STATE::IGS_IN_MAP: Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "当前场景确定为游戏地图。"); break;
        case IN_GAME_STATE::IGS_LOADING: Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "当前场景确定为游戏加载界面。"); break;
        case IN_GAME_STATE::IGS_IN_ROOM: Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "当前场景确定为游戏房间。"); break;
        default: break;
        }
    }
    /* 非法状态迁移，4 种 */
    else if (state.GetState() == IN_GAME_STATE::IGS_LOADING && current_state_typename == IN_GAME_STATE::IGS_IN_ROOM) /* 加载失败，现象一般为重连 1 2 3，可能是网络问题 */
    {
        abnormal_back_to_room++;
        if (abnormal_back_to_room == 3)
        {
            state.update(IN_GAME_STATE::IGS_IN_HALL, current_time); /* 直接离开房间回到大厅 */
            Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_WARNING, "异常回到游戏房间次数过多，离开当前房间重新创建。");
            abnormal_back_to_room = 0;
        }
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_WARNING, "游戏场景加载失败（可能为网络问题，一般表现为重连 1 2 3）。");
    }
    else if (state.GetState() == IN_GAME_STATE::IGS_IN_MAP && current_state_typename == IN_GAME_STATE::IGS_IN_HALL) /* 从游戏场景返回到大厅，原因一般为：强制踢出、长时间没有有效操作 */
    {
        state.update(IN_GAME_STATE::IGS_IN_HALL, current_time);
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_WARNING, "从游戏场景返回到大厅（可能是被强踢或长时间没有有效操作，若是自行退出请忽略）。");
    }
    //else if (state.GetState() == IN_GAME_STATE::IGS_IN_MAP && current_state_typename == IN_GAME_STATE::IGS_IN_ROOM) /* 从游戏场景返回到房间，原因一般为连接游戏服务器超时 */
    //{
    //    abnormal_back_to_room++;
    //    if (abnormal_back_to_room == 3)
    //    {
    //        state.update(IN_GAME_STATE::IGS_IN_HALL, current_time); /* 直接离开房间回到大厅 */
    //        abnormal_back_to_room = 0;
    //        Console::Log(CONSOLE_LOG_LEVEL::CLL_WARNING, "异常回到游戏房间次数过多，离开当前房间重新创建。");
    //    }
    //    Console::Log(CONSOLE_LOG_LEVEL::CLL_WARNING, "从游戏地图返回到房间（可能是与游戏服务器连接超时）。");
    //}
    else if (state.GetState() == IN_GAME_STATE::IGS_IN_ROOM && current_state_typename == IN_GAME_STATE::IGS_IN_HALL) /* 从游戏房间返回到大厅，原因可能是：强制踢出、房间等待时间过长 */
    {
        state.update(IN_GAME_STATE::IGS_IN_HALL, current_time);
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_WARNING, "从游戏房间返回到大厅（可能是被强踢或房间等待时间过长，若是自行离开请忽略）。");
    }
    else /* 其他情形，除非手动操作，否则应该不会出现 */
    {
        state.update(current_state_typename, current_time);
        switch (current_state_typename)
        {
        case IN_GAME_STATE::IGS_IN_HALL: Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "当前场景确定为游戏大厅。"); break;
        case IN_GAME_STATE::IGS_IN_MAP: Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "当前场景确定为游戏地图。"); break;
        case IN_GAME_STATE::IGS_LOADING: Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "当前场景确定为游戏加载界面。"); break;
        case IN_GAME_STATE::IGS_IN_ROOM: Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "当前场景确定为游戏房间。"); break;
        default: break;
        }
    }

    /* 超时检查 */
    if (state.GetState() == IN_GAME_STATE::IGS_IN_ROOM && current_time - state.GetTimestamp() > s_Instance->GetMaxWaitTimeInGameRoom())
    {
        state.update(IN_GAME_STATE::IGS_IN_HALL, current_time);
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_WARNING, "在房间内等待时间超过预设 %u 秒，回到大厅重新创建房间。", s_Instance->GetMaxWaitTimeInGameRoom());
    }
	else if (state.GetState() == IN_GAME_STATE::IGS_LOGIN && current_time - state.GetTimestamp() > 60) /* 登录时间超过 60 秒 */
	{
        state.update(IN_GAME_STATE::IGS_UNKNOWN, current_time);
		Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_WARNING, "等待游戏登录达到超时时间（60 秒）。");
	}
    else if (state.GetState() == IN_GAME_STATE::IGS_LOADING && current_time - state.GetTimestamp() > 150) /* 游戏加载超时 */
    {
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_WARNING, "等待游戏加载达到超时时间（超时设定为 150 秒，超过该时间可能是游戏失去响应），结束游戏进程以重新启动。");
		std::lock_guard lk(s_Instance->m_GameInfoMutex);
		TerminateProcess(s_Instance->m_hGameProcess.get(), 0);
    }
}

void Controller::DispatchAutoPlayCommand()
{
    auto remove_game_window_border = []() {
        std::lock_guard lk(s_Instance->m_GameInfoMutex);
        if (GetWindowLongPtrW(s_Instance->m_hGameWindow, GWL_STYLE) & WS_CAPTION)
        {
			auto MakeWindowBorderless = reinterpret_cast<void (*) (HWND)>(GetProcAddress(s_Instance->m_hDllMod, "MakeWindowBorderless"));
			if (MakeWindowBorderless)
			{
				MakeWindowBorderless(s_Instance->m_hGameWindow);
			}
        }
	};
    auto& m_CurrentState = s_Instance->m_CurrentState;
    std::time_t current_time = std::time(nullptr);
    if (m_CurrentState.GetState() == IN_GAME_STATE::IGS_UNKNOWN)
    {
        thread_local ExecutorCommand cmd(5, true);
        cmd.Set(EXECUTOR_COMMAND::CMD_CLEAR_POPUPS, current_time);
        s_Instance->m_Messenger.Dispatch(cmd);
    }
    else if (m_CurrentState.GetState() == IN_GAME_STATE::IGS_LOGIN) /* 正在登陆 */
    {
		s_Instance->m_Messenger.DispatchNOP();
    }
    else if (m_CurrentState.GetState() == IN_GAME_STATE::IGS_IN_HALL) /* 大厅内 */
    {
        /* 创建一个房间的耗时在 15 秒左右，进行一次状态判定的耗时从 2 ~ 6 秒不等，具体取决于硬件配置 */
        /* 假定某一段时间内状态判定耗时均为 τ 秒，那么为了使申请到新 ID 时的判定的状态是准确的，则： */
        /* 申请分配新命令 ID 的间隔时间应为 15 + τ */
        /* 保守起见，这里将申请新 ID 的间隔设置为 30 秒 */
        thread_local ExecutorCommand cmd(30, false);
        cmd.Set(EXECUTOR_COMMAND::CMD_CREATE_GAME_ROOM, current_time);
        remove_game_window_border();
        s_Instance->m_Messenger.Dispatch(cmd);
    }
    else if (m_CurrentState.GetState() == IN_GAME_STATE::IGS_IN_ROOM) /* 房间内（正常） */
    {
        thread_local ExecutorCommand cmd(3, true);
        cmd.Set(EXECUTOR_COMMAND::CMD_START_GAME_ROOM, current_time);
        remove_game_window_border();
        s_Instance->m_Messenger.Dispatch(cmd);
    }
    else if (m_CurrentState.GetState() == IN_GAME_STATE::IGS_LOADING) /* 加载 */
    {
        s_Instance->m_Messenger.DispatchNOP();
    }
    else if (m_CurrentState.GetState() == IN_GAME_STATE::IGS_IN_MAP) /* 游戏中 */
    {
        thread_local ExecutorCommand cmd(3, true);
        if (s_Instance->m_ExtendedAutoPlayMode && current_time - m_CurrentState.GetTimestamp() > 60)
        {
			cmd.Set(EXECUTOR_COMMAND::CMD_EXTENDED_IDLE, current_time);
        }
        else if (current_time - m_CurrentState.GetTimestamp() > 5)
        {
			cmd.Set(EXECUTOR_COMMAND::CMD_DEFAULT_IDLE, current_time);
        }
        else
        {
            cmd.Set(EXECUTOR_COMMAND::CMD_CHOOSE_CHARACTER, current_time);
        }
        remove_game_window_border();
        s_Instance->m_Messenger.Dispatch(cmd);
    }
}

void Controller::WatchInGameState() noexcept
{
    try
    {
        EventList event_list{
            &s_Instance->m_InGameStateWatcherSwitch,
            &s_Instance->m_GameProcessAlive,
        };
        while (true)
        {
            auto start = std::chrono::system_clock::now();
            event_list.WaitAll();
            if (s_Instance->m_ExitThreads)
            {
                break;
            }
            s_Instance->m_InGameStateWatcherFinished.Reset(); /* 一定是在等待返回后才开始执行，并报告 */
            AnalyzeInGameState();
            DispatchAutoPlayCommand();
            s_Instance->m_InGameStateWatcherFinished.Set();
            auto end = std::chrono::system_clock::now();
            auto elapse = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            auto sleep_time = elapse > std::chrono::seconds(6) ? std::chrono::milliseconds(500) : std::chrono::seconds(6) - elapse;
            std::this_thread::sleep_for(sleep_time);
        };
    }
    catch (Exception &e)
    {
        Console::Log(e.GetLevel(), e.what());
    }
    catch (std::exception& e)
    {
        Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_ERROR, "%s", e.what());
    }
    Console::Log(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_MESSAGE, "线程 m_InGameStateWatcher 退出。");
    s_Instance->m_ThreadExitEvent.Set();
}
}
