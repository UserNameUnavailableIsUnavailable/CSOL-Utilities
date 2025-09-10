#pragma once

#include "Module.hpp"

namespace CSOL_Utilities
{
    class GameProcessInformation
    {
        const std::wstring process_name_; // 进程名称
        const std::wstring window_title_; // 窗口标题
        const std::wstring executable_path_; // 可执行文件路径
        const std::wstring launch_command_; // 启动命令
        std::atomic<HWND> window_handle_ = nullptr; // 窗口句柄，会被多个线程访问
        std::atomic<DWORD> process_id_ = 0; // 进程 ID
        std::atomic<HANDLE> process_handle_ = nullptr; // 进程句柄
    public:
		GameProcessInformation(std::wstring game_process_name, std::wstring game_window_title, std::wstring game_executable_path, std::wstring game_process_launch_command) :
			process_name_(std::move(game_process_name)), window_title_(std::move(game_window_title)), executable_path_(std::move(game_executable_path)), launch_command_(std::move(game_process_launch_command))
		{
		}
		GameProcessInformation(const GameProcessInformation& game_process_information) = delete;
        GameProcessInformation(GameProcessInformation&& game_process_information) = delete;
        void clear() noexcept
        {
            set_process_id(0);
            set_window_handle(nullptr);
            set_process_handle(nullptr);
        }
        const std::wstring& get_process_name() const noexcept
        {
            return process_name_;
        }
        const std::wstring& get_window_title() const noexcept
        {
            return window_title_;
        }
        const std::wstring& get_executable_path() const noexcept
        {
            return executable_path_;
        }
        const std::wstring& get_launch_command() const noexcept
        {
            return launch_command_;
        }
        HWND get_window_handle() const noexcept
        {
            return window_handle_.load(std::memory_order_acquire);
        }
        void set_window_handle(HWND hWnd) noexcept
        {
            window_handle_.store(hWnd, std::memory_order_release);
        }
        DWORD get_process_id() const noexcept
        {
            return process_id_.load(std::memory_order_acquire);
        }
        void set_process_id(DWORD pid) noexcept
        {
            process_id_.store(pid, std::memory_order_release);
        }
        HANDLE get_process_handle() const noexcept
        {
            return process_handle_.load(std::memory_order_acquire);
        }
        void set_process_handle(HANDLE hProcess) noexcept
        {
            // 执行原子交换，确保旧的进程句柄被正确关闭，防止资源泄漏
            auto h = process_handle_.exchange(hProcess, std::memory_order_acq_rel);
            if (h != nullptr && h != INVALID_HANDLE_VALUE)
            {
                CloseHandle(h);
            }
        }
    };

	/* 游戏状态 */
	enum class GAME_INTERFACE_TYPE
	{
		LOGIN, /* 登陆界面 */
		LOBBY, /* 大厅界面 */
		ROOM, /* 房间界面 */
		LOADING, /* 加载界面 */
		IN_GAME, /* 游戏内界面 */
        RESULTS, /* 战绩界面 */
		UNKNOWN /* 未知界面 */
	};

    constexpr static GAME_INTERFACE_TYPE StringToInterfaceType(std::string_view description) noexcept
    {
        if (description == "LOGIN")
        {
            return GAME_INTERFACE_TYPE::LOGIN;
        }
        else if (description == "LOBBY")
        {
            return GAME_INTERFACE_TYPE::LOBBY;
        }
        else if (description == "ROOM")
        {
            return GAME_INTERFACE_TYPE::ROOM;
        }
        else if (description == "LOADING")
        {
            return GAME_INTERFACE_TYPE::LOADING;
        }
        else if (description == "IN_GAME")
        {
            return GAME_INTERFACE_TYPE::IN_GAME;
        }
        else if (description == "RESULTS")
        {
            return GAME_INTERFACE_TYPE::RESULTS;
        }
        else
        {
            return GAME_INTERFACE_TYPE::UNKNOWN;
        }
    }

    constexpr static std::string_view InterfaceTypeToString(GAME_INTERFACE_TYPE t) noexcept
    {
        switch (t)
        {
        case GAME_INTERFACE_TYPE::LOGIN:
            return "LOGIN";
        case GAME_INTERFACE_TYPE::LOBBY:
            return "LOBBY";
        case GAME_INTERFACE_TYPE::ROOM:
            return "ROOM";
        case GAME_INTERFACE_TYPE::LOADING:
            return "LOADING";
        case GAME_INTERFACE_TYPE::IN_GAME:
            return "IN_GAME";
        case GAME_INTERFACE_TYPE::RESULTS:
            return "RESULTS";
        case GAME_INTERFACE_TYPE::UNKNOWN:
        default:
            return "UNKNOWN";
        }
    }

    using InterfaceTransition = std::pair<GAME_INTERFACE_TYPE, GAME_INTERFACE_TYPE>;

    struct InterfaceTransitionHash
    {
        constexpr std::uint64_t operator()(const InterfaceTransition& t) const
        {
            std::uint64_t left = static_cast<std::uint64_t>(t.first);
            std::uint64_t right = static_cast<std::uint64_t>(t.second);
            assert(left <= UINT32_MAX && right <= UINT32_MAX); // enum 中的元素数值不应超过 UINT32_MAX
            return (left << 32) | right;
        }
    };

    /* 游戏进程状态 */
	enum class GAME_PROCESS_STATE
	{
		BEING_CREATED, /* 游戏进程正在被创建 */
		RUNNING, /* 游戏进程正在运行 */
		EXITED, /* 游戏进程退出 */
		UNKNOWN, /* 尚未确认游戏进程状态 */
	};

    enum class IDLE_MODE
    {
	    DEFAULT,
        EXTENDED
    };

    enum class ABNORMAL_TYPE
    {
        LOGIN_TIMEOUT, // 登录超时
        LOADING_TIMEOUT, // 游戏场景加载失败
        RETURN_FROM_IN_GAME_TO_ROOM_TOO_MANY_TIMES, // 从游戏中返回到游戏房间连续多次
        RETURN_FROM_IN_GAME_TO_LOBBY, // 从游戏中返回到游戏大厅
        RETURN_FROM_ROOM_TO_LOBBY, // 从房间中返回到游戏大厅
        WAIT_START_GAME_ROOM_TIMEOUT, // 等待开始游戏超时
    };

    class IdleEngine : public Module
    {
    public:
        IdleEngine(std::unique_ptr<GameProcessInformation> game_process_info);
        ~IdleEngine() noexcept;

        virtual void Boot();
        virtual void Resume() noexcept;
        virtual void Suspend() noexcept;
        virtual void Terminate() noexcept;
		void SetDiscriminationInterval(uint32_t interval) noexcept
        {
            discrimination_interval_.store(interval, std::memory_order_release);
        }
        uint32_t GetDiscriminationInterval() const noexcept
        {
            return discrimination_interval_.load(std::memory_order_acquire);
        }
        void SetWatchdogInterval(uint32_t interval) noexcept
        {
            watchdog_interval_.store(interval, std::memory_order_release);
        }
        uint32_t GetWatchdogInterval() const noexcept
        {
            return watchdog_interval_.load(std::memory_order_acquire);
        }
        void SetIdleMode(IDLE_MODE idle_mode) noexcept
        {
            idle_mode_.store(idle_mode, std::memory_order_release);
        }
		IDLE_MODE GetIdleMode() const noexcept
        {
			return idle_mode_.load(std::memory_order_acquire);
        }
        virtual void ResetAfterSwitchMode() = 0; /* 切换状态后重置状态机 */
        virtual void ResetAfterReconnection() = 0; /* 完成掉线重连后重置状态机 */
    protected:
        // 生命周期控制
        std::mutex boot_lock_; /* 引擎启动锁 */
        bool is_booted_ = false; /* 引擎是否启动 */
        // 线程控制
        std::mutex threads_state_lock_; /* 判别器线程和监视器线程之间的状态同步锁 */
        std::stop_source stop_source_; /* 控制线程启停 */
        std::condition_variable process_watcher_runnable_; /* 进程监视器可运行条件 */
        bool is_process_watcher_runnable_ = false; /* 进程监视器是否可运行 */
        std::condition_variable process_watcher_finished_; /* 进程监视器一轮运行结束 */
        bool has_process_watcher_finished_ = true; /* 进程监视器是否完成当前轮次 */
        std::condition_variable scene_discriminator_runnable_; /* 场景判别器可运行 */
        bool is_scene_discriminator_runnable_ = false; /* 场景判别器是否可运行 */
        std::condition_variable scene_discriminator_finished_; /* 场景判别器一轮运行结束 */
        bool has_scene_discriminator_finished_ = true; /* 场景判别器是否完成当前轮次 */
        // 功能实现
        bool SearchGameWindow(); /* 辅助函数：查找游戏窗口，更新游戏进程信息 */
        std::thread process_watcher_; /* 游戏进程监视器线程 */
        GAME_PROCESS_STATE process_state_ = GAME_PROCESS_STATE::UNKNOWN;
        virtual void Watch(); /* 监视游戏进程 */
        std::thread scene_discriminator_; /* 游戏状态判别器线程 */
        virtual void Discriminate() = 0; /* 判别游戏状态 */
        /* 考虑到 dangling resources 问题，这里使用 thread 而非 jthread */
        std::unique_ptr<GameProcessInformation> game_process_info_; /* 游戏进程信息 */
        std::atomic<IDLE_MODE> idle_mode_; /* 挂机模式 */
        std::atomic_uint32_t discrimination_interval_ = 5000; /* 状态判别间隔，单位毫秒 */
        std::atomic_uint32_t watchdog_interval_ = 1000; /* 进程监视器轮询间隔，单位毫秒 */
    };
}
