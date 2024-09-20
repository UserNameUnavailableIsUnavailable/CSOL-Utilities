#pragma once
#include "CEvent.hpp"
#include "CSOL_Utilities.hpp"
#include "CMessenger.hpp"
#include <Windows.h>
#include <minwindef.h>
#include <thread>
#include <mutex>
#include <filesystem>

class CSOL_Utilities::CCommander
{
public:
    static inline void InitializeInstance(std::string sGameRootPath, std::string sGameLauncherPath);
    static CCommander& RetrieveInstance();
    static inline void DestroyInstance() noexcept;
private:
    CCommander(std::string sGameRootPath, std::string sGameLauncherPath);
    ~CCommander() noexcept;
    CCommander(const CCommander&) = delete;
    CCommander(const CCommander&&) = delete;
    CCommander& operator=(CCommander&) = delete;
    CCommander& operator=(CCommander&&) = delete;
private:
    static CCommander* s_Instance;
    /* 状态标志 */
    static std::mutex s_singletonStateMutex;
    bool m_bExitThreads = false;
    std::time_t m_Bias; /* UTC - LTC，单位为秒 */
    std::filesystem::path m_GameRootPath; /* 游戏根目录 */
    std::filesystem::path m_GameLauncherPath; /* 启动游戏命令行 */
    /* 处理热键 */
    static void HandleHotKeyEvent() noexcept; /* 处理热键 */
    std::thread m_HotKeyEventHandler; /* 热键处理线程 */
    /* 挂机 */
    static void WatchInGameState() noexcept; /* 监测游戏内状态 */
    static CInGameState& ResolveState(); /* 解析状态 */
    static void TransferState() noexcept; /* 状态迁移 */
    std::thread m_InGameStateWatcher; /* 监测游戏内状态的线程对象 */
    CEvent m_InGameStateWatcherSwitch; /* 当 m_InGameStateWatcher 持有此对象时才能运行 */
    CEvent m_GameProcessAlive; /* 当 m_InGameStateWatcher 持有此对象时才能运行 */
    CEvent m_InGameStateWatcherFinished; /* 用于告知热键处理例程挂起状态 */
    static void WatchGameProcess() noexcept; /* 监测游戏进程状态 */
    std::thread m_GameProcessWatcher; /* 监测游戏进程状态的线程对象 */
    CEvent m_GameProcessWatcherSwitch; /* 当 m_GameProcessWatcher 持有此对象时才能运行 */
    CEvent m_GameProcessWatcherFinished; /* 用于告知热键处理例程挂起状态 */
    bool m_bWasGameProcessWatcherInterrupted; /* m_GameProcessWatcher 是否曾被中断 */
    CSOL_Utilities::CMessenger m_Messenger; /* 向 Lua Executor 下达命令 */
    HMODULE hToolModule = nullptr;
    HANDLE m_hGameProcess = nullptr; /* 游戏进程句柄 */
    HWND m_hGameWindow = nullptr; /* 游戏窗口句柄 */
};