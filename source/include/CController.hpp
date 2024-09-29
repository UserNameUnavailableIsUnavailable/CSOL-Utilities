#pragma once
#include "CEvent.hpp"
#include "CInGameState.hpp"
#include "CMessenger.hpp"
#include "CSOL_Utilities.hpp"
#include <memory>
#ifdef WIN32
#include <Windows.h>
#endif
#include <filesystem>
#include <mutex>
#include <thread>

namespace CSOL_Utilities
{
class CController
{
  public:
    void RunInstance();
    static void InitializeInstance(std::string sGameRootPath, std::string sGameLauncherPath);
    static CController &RetrieveInstance();
    static void DestroyInstance() noexcept;
    void SetMaxWaitTimeInGameRoom(uint32_t t)
    {
        m_MaxWaitTimeInGameRoom = t > 60 ? t : 60;
    };
    uint32_t GetMaxWaitTimeInGameRoom()
    {
        return m_MaxWaitTimeInGameRoom;
    };

  private:
    CController(std::string sGameRootPath, std::string sGameLauncherPath);
    ~CController() noexcept;
    CController(const CController &) = delete;
    CController(const CController &&) = delete;
    CController &operator=(CController &) = delete;
    CController &operator=(CController &&) = delete;

  private:
    static CController *s_Instance;
    /* 状态标志 */
    static std::mutex s_SingletonStateMutex;
    bool m_ExitThreads{false};
    CMannualEvent m_ThreadExitEvent{};          /* 当有线程退出时，设置此事件 */
    std::time_t m_Bias;                         /* UTC - LTC，单位为秒 */
    std::filesystem::path m_GameRootPath;       /* 游戏根目录 */
    std::shared_ptr<wchar_t[]> m_LaunchGameCmd; /* 启动游戏命令行 */
    /* 依赖的外部 DLL */
    HMODULE m_hDllMod{nullptr}; /* GamingTool.dll */
    /* 处理热键 */
    static void HandleHotKeyEvent() noexcept; /* 处理热键 */
    std::thread m_HotKeyEventHandler{};       /* 热键处理线程 */
    /* 挂机 */
    uint32_t m_MaxWaitTimeInGameRoom{15 * 60};
    bool m_ExtendedAutoPlayMode{false};
    inline void SetExtendedAutoPlayMode(bool toggle) noexcept
    {
        m_ExtendedAutoPlayMode = toggle;
    };
    static void WatchInGameState() noexcept;                                 /* 监测游戏内状态 */
    static CInGameState &ResolveState();                                     /* 解析状态 */
    static void DispatchCommand(const CInGameState &in_game_state) noexcept; /* 下达命令 */
    static void TransferState() noexcept;                                    /* 状态迁移 */
    std::thread m_InGameStateWatcher{};                                      /* 监测游戏内状态的线程对象 */
    CMannualEvent m_InGameStateWatcherSwitch{};   /* 当 m_InGameStateWatcher 持有此对象时才能运行 */
    CMannualEvent m_GameProcessAlive{};           /* 当 m_InGameStateWatcher 持有此对象时才能运行 */
    CMannualEvent m_InGameStateWatcherFinished{}; /* 用于告知热键处理例程挂起状态 */
    static void WatchGameProcess() noexcept;      /* 监测游戏进程状态 */
    std::thread m_GameProcessWatcher{};           /* 监测游戏进程状态的线程对象 */
    CMannualEvent m_GameProcessWatcherSwitch{};   /* 当 m_GameProcessWatcher 持有此对象时才能运行 */
    CMannualEvent m_GameProcessWatcherFinished{}; /* 用于告知热键处理例程挂起状态 */
    bool m_bWasGameProcessWatcherInterrupted{};   /* m_GameProcessWatcher 是否曾被中断 */
    CMessenger m_Messenger;                       /* 向 Lua Executor 下达命令 */
    HWND m_hGameWindow = nullptr;                 /* 游戏窗口句柄 */
    /* 定期下达固定命令的线程，适用于简单的场景 */
    CMannualEvent m_FixedCommandDispatcherSwitch{};
    std::thread m_FixedCommandDispatcher{};
    COMMAND m_Command{COMMAND::CMD_NOP};
    CMannualEvent m_FixedCommandDispatcherFinished{};
    static void DispatchFixedCommand(COMMAND &command) noexcept;
};
}; // namespace CSOL_Utilities