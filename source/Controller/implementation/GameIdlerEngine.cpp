#include <filesystem>
#include "IdlerEngine.hpp"
#include "Exception.hpp"
#undef max
#undef min
#include "aho_corasick/aho_corasick.hpp"

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
    
    #define UPDATE_STATE(state, tp) \
        m_igs = state;\
        m_tp = tp;

    void IdlerEngine::analyze()
    {
        thread_local HWND hGameWindow = NULL;
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
        std::chrono::system_clock::time_point tp;
        if (!m_hGameWindow || !IsWindow(m_hGameWindow))
        {
            return;
        }
        if (IsIconic(m_hGameWindow))
        {
            ShowWindow(m_hGameWindow, SW_NORMAL);
        }
        /* 窗口置顶 */
        SetWindowPos(m_hGameWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        tp = std::chrono::system_clock::now(); // 获取当前时刻

        // todo: 截图
    // 
        thread_local OCR_PARAM param{ 0 };
        if (!std::filesystem::is_regular_file(GAME_IMAGE_FILE_NAME_UTF16))
        {
            return;
        }
        auto recognition_start = std::chrono::system_clock::now();
        OCR_BOOL bRet = OcrDetect(m_hOcr, "./", GAME_IMAGE_FILE_NAME_UTF8, &param);

        thread_local std::int32_t buffer_size{ 8192 };
        assert(buffer_size > 0);
        thread_local std::unique_ptr<char> pBuffer(new char[buffer_size]);
        if (!bRet)
        {
            return;
        }
        std::int64_t len = OcrGetLen(m_hOcr);
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
        if (!OcrGetResult(m_hOcr, pBuffer.get(), buffer_size))
        {
            return;
        }

        IN_GAME_STATE current_state;

        auto result = trie.parse_text(pBuffer.get());
        if (result.empty())
        {
            current_state = IN_GAME_STATE::IGS_UNKNOWN;
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
                current_state = (*result)->state;
            }
        }

        thread_local int abnormal_back_to_room = 0;
        /* 合法状态迁移 */
        /* 手写状态机，考虑到后续维护难度，故引入了更多的代码冗余降低理解难度 */
        /* 5 种自迁移 */
        if (m_igs == current_state)
        {

        }
        /* 5 种顺序迁移 */
        else if (m_igs == IN_GAME_STATE::IGS_LOGIN && current_state == IN_GAME_STATE::IGS_IN_HALL) /* 登陆成功，进入大厅 */
        {
            UPDATE_STATE(current_state, tp);
            Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "登陆成功，进入大厅。");
        }
        else if (m_igs == IN_GAME_STATE::IGS_IN_HALL && current_state == IN_GAME_STATE::IGS_IN_ROOM) /* 创建房间 */
        {
            UPDATE_STATE(current_state, tp);
            Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "进入游戏房间。");
        }
        else if (m_igs == IN_GAME_STATE::IGS_IN_ROOM && current_state == IN_GAME_STATE::IGS_LOADING) /* 开始游戏 */
        {
            UPDATE_STATE(current_state, tp);
            Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "开始游戏，加载游戏场景。");
        }
        else if (m_igs == IN_GAME_STATE::IGS_LOADING && current_state == IN_GAME_STATE::IGS_IN_MAP) /* 场景加载完成 */
        {
            UPDATE_STATE(current_state, tp);
            Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "游戏场景加载完成。");
        }
        else if (m_igs == IN_GAME_STATE::IGS_IN_MAP && current_state == IN_GAME_STATE::IGS_IN_ROOM) /* 结算确认 */
        {
            UPDATE_STATE(current_state, tp);
            Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "结算确认，回到游戏房间。");
        }
        /* 消除未知状态迁移，不计入迁移种类 */
        else if (current_state == IN_GAME_STATE::IGS_UNKNOWN)
        {
            if (m_igs != IN_GAME_STATE::IGS_IN_MAP && m_igs != IN_GAME_STATE::IGS_LOADING)
            {
                UPDATE_STATE(IN_GAME_STATE::IGS_UNKNOWN, tp);
            }
            else
            {
                /* 正在游戏 */
            }
        }
        else if (m_igs == IN_GAME_STATE::IGS_UNKNOWN && current_state != IN_GAME_STATE::IGS_UNKNOWN)
        {
            UPDATE_STATE(current_state, tp);
            switch (current_state)
            {
            case IN_GAME_STATE::IGS_IN_HALL: Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "当前场景确定为游戏大厅。"); break;
            case IN_GAME_STATE::IGS_IN_MAP: Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "当前场景确定为游戏地图。"); break;
            case IN_GAME_STATE::IGS_LOADING: Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "当前场景确定为游戏加载界面。"); break;
            case IN_GAME_STATE::IGS_IN_ROOM: Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "当前场景确定为游戏房间。"); break;
            default: break;
            }
        }
        /* 非法状态迁移，4 种 */
        else if (m_igs == IN_GAME_STATE::IGS_LOADING && current_state == IN_GAME_STATE::IGS_IN_ROOM) /* 加载失败，现象一般为重连 1 2 3，可能是网络问题 */
        {
            abnormal_back_to_room++;
            if (abnormal_back_to_room == 3)
            {
                UPDATE_STATE(IN_GAME_STATE::IGS_IN_HALL, tp); /* 直接离开房间回到大厅 */
                Console::Log(EXCEPTION_LEVEL::EL_WARNING, "异常回到游戏房间次数过多，离开当前房间重新创建。");
                abnormal_back_to_room = 0;
            }
            Console::Log(EXCEPTION_LEVEL::EL_WARNING, "游戏场景加载失败（可能为网络问题，一般表现为重连 1 2 3）。");
        }
        else if (m_igs == IN_GAME_STATE::IGS_IN_MAP && current_state == IN_GAME_STATE::IGS_IN_HALL) /* 从游戏场景返回到大厅，原因一般为：强制踢出、长时间没有有效操作 */
        {
            UPDATE_STATE(IN_GAME_STATE::IGS_IN_HALL, tp);
            Console::Log(EXCEPTION_LEVEL::EL_WARNING, "从游戏场景返回到大厅（可能是被强踢或长时间没有有效操作，若是自行退出请忽略）。");
        }
        else if (m_igs == IN_GAME_STATE::IGS_IN_ROOM && current_state == IN_GAME_STATE::IGS_IN_HALL) /* 从游戏房间返回到大厅，原因可能是：强制踢出、房间等待时间过长 */
        {
            UPDATE_STATE(IN_GAME_STATE::IGS_IN_HALL, tp);
            Console::Log(EXCEPTION_LEVEL::EL_WARNING, "从游戏房间返回到大厅（可能是被强踢或房间等待时间过长，若是自行离开请忽略）。");
        }
        else /* 其他情形，除非手动操作，否则应该不会出现 */
        {
            UPDATE_STATE(current_state, tp);
            switch (current_state)
            {
            case IN_GAME_STATE::IGS_IN_HALL: Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "当前场景确定为游戏大厅。"); break;
            case IN_GAME_STATE::IGS_IN_MAP: Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "当前场景确定为游戏地图。"); break;
            case IN_GAME_STATE::IGS_LOADING: Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "当前场景确定为游戏加载界面。"); break;
            case IN_GAME_STATE::IGS_IN_ROOM: Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, "当前场景确定为游戏房间。"); break;
            default: break;
            }
        }

        /* 超时检查 */
        if (m_igs == IN_GAME_STATE::IGS_IN_ROOM && tp - m_tp > std::chrono::seconds(g_MaxWaitTimeInGameRoom))
        {
            UPDATE_STATE(IN_GAME_STATE::IGS_IN_HALL, tp);
            Console::Log(EXCEPTION_LEVEL::EL_WARNING, "在房间内等待时间超过预设 %u 秒，回到大厅重新创建房间。", g_MaxWaitTimeInGameRoom);
        }
        else if (m_igs == IN_GAME_STATE::IGS_LOGIN && tp - m_tp > std::chrono::seconds(60)) /* 登录时间超过 60 秒 */
        {
            UPDATE_STATE(IN_GAME_STATE::IGS_UNKNOWN, tp);
            Console::Log(EXCEPTION_LEVEL::EL_WARNING, "等待游戏登录达到超时时间（60 秒）。");
        }
        else if (m_igs == IN_GAME_STATE::IGS_LOADING && tp - m_tp > std::chrono::seconds(150)) /* 游戏加载超时 */
        {
            Console::Log(EXCEPTION_LEVEL::EL_WARNING, "等待游戏加载达到超时时间（超时设定为 150 秒，超过该时间可能是游戏失去响应），结束游戏进程并重新启动。");
            DWORD dwPId = 0;
            GetWindowThreadProcessId(hGameWindow, &dwPId);
            if (dwPId)
            {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, NULL, dwPId);
                if (hProcess)
                {
                    TerminateProcess(hProcess, -1);
                    CloseHandle(hProcess);
                }
            }
        }
    }

    void IdlerEngine::dispatch()
    {
        thread_local COMMAND last_dispatched_command = COMMAND::CMD_NOP;
        COMMAND command;
        // #define NO_NEED_TO_REDISPATCH(command) (command == last_dispatched_command) 
    
        auto remove_game_window_border = [this]() {
            auto MakeWindowBorderless = reinterpret_cast<void (*) (HWND)>(GetProcAddress(m_hGamingTool, "MakeWindowBorderless"));
            if (MakeWindowBorderless)
            {
                MakeWindowBorderless(m_hGameWindow);
            }
        };

        std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
        if (m_igs == IN_GAME_STATE::IGS_UNKNOWN)
        {
            command = COMMAND::CMD_CLEAR_POPUPS;
            m_cmd->set(COMMAND::CMD_CLEAR_POPUPS, true);
        }
        else if (m_igs == IN_GAME_STATE::IGS_LOGIN) /* 正在登陆 */
        {
            command = COMMAND::CMD_NOP;
            m_cmd->nop();
        }
        else if (m_igs == IN_GAME_STATE::IGS_IN_HALL) /* 大厅内 */
        {
            /* 创建一个房间的耗时在 15 秒左右，进行一次状态判定的耗时从 2 ~ 6 秒不等，具体取决于硬件配置 */
            /* 假定某一段时间内状态判定耗时均为 τ 秒，那么为了使申请到新 ID 时的判定的状态是准确的，则： */
            /* 申请分配新命令 ID 的间隔时间应为 15 + τ */
            /* 保守起见，这里将申请新 ID 的间隔设置为 30 秒 */
            command = COMMAND::CMD_CREATE_GAME_ROOM;
            thread_local std::chrono::time_point<std::chrono::system_clock> last_tp;
            if (tp - last_tp > std::chrono::seconds(30))
            {
                last_tp = tp;
            }
        }
        else if (m_igs == IN_GAME_STATE::IGS_IN_ROOM) /* 房间内（正常） */
        {
            command = COMMAND::CMD_START_GAME_ROOM;
        }
        else if (m_igs == IN_GAME_STATE::IGS_LOADING) /* 加载 */
        {
            command = COMMAND::CMD_NOP;
        }
        else if (m_igs == IN_GAME_STATE::IGS_IN_MAP) /* 游戏中 */
        {
            if (m_extended.load(std::memory_order_acquire) && tp - m_tp > std::chrono::milliseconds(60))
            {
                command = COMMAND::CMD_EXTENDED_IDLE;
            }
            else if (tp - m_tp > std::chrono::seconds(5))
            {
                command = COMMAND::CMD_DEFAULT_IDLE;
            }
            else
            {
                command = COMMAND::CMD_CHOOSE_CHARACTER;
            }
        }
        remove_game_window_border();
        if (command == last_dispatched_command)
        {
            return;
        }
        #undef NO_NEED_TO_REDISPATCH
        last_dispatched_command = command;
        switch (command)
        {
        case COMMAND::CMD_NOP: m_cmd->nop(); break;
        case COMMAND::CMD_START_GAME_ROOM: m_cmd->set(command, true); break;
        case COMMAND::CMD_CHOOSE_CHARACTER: m_cmd->set(command, true); break;
        case COMMAND::CMD_DEFAULT_IDLE: m_cmd->set(command, true); break;
        case COMMAND::CMD_EXTENDED_IDLE: m_cmd->set(command, true); break;
        case COMMAND::CMD_CONFIRM_RESULTS: m_cmd->set(command, true); break;
        case COMMAND::CMD_CREATE_GAME_ROOM: m_cmd->set(command, false); break;
        case COMMAND::CMD_CLEAR_POPUPS: m_cmd->set(command, false); break;
        default:
            throw Exception("意料外的命令：%s。", Command::query_command_string(command));
        }
    }

    void IdlerEngine::work()
    {
        analyze();
        dispatch();
    }
}
