#pragma once
#include "CSOL_Utilities.hpp"
#include "CMessenger.hpp"
#include <Windows.h>
#include <thread>
#include <mutex>
#include <filesystem>
#include <fstream>

/*C
CCSOLUtilities 类，单例模式。
C*/
class CSOL_Utilities::CCommander
{
public:
    static inline void InitializeInstance(std::string sGameRootPath, std::string sGameLauncherPath);
    static inline CCommander& RetrieveObject() noexcept;
    static inline void DestroyInstance() noexcept;
private:
    CCommander(std::string sGameRootPath, std::string sGameLauncherPath);
    ~CCommander() noexcept;
    CCommander(const CCommander&) = delete;
    CCommander(const CCommander&&) = delete;
    CCommander& operator=(CCommander&) = delete;
    CCommander& operator=(CCommander&&) = delete;
private:
    static CCommander* s_controller;
    /* 状态标志 */
    static std::mutex s_singletonStateMutex;
    bool m_bExitThreads = false;
    std::filesystem::path m_GameRootPath; /* 游戏根目录 */
    std::filesystem::path m_GameLauncherPath; /* 启动游戏命令行 */
    /* 处理热键 */
    static void HandleHotKeyEvent() noexcept; /* 处理热键 */
    std::thread m_HotKeyHandler; /* 热键处理线程 */
    /* 挂机 */
    static void WatchInGameState() noexcept; /* 监测游戏内状态 */
    std::thread m_InGameStateWatcher; /* 监测游戏内状态的线程对象 */
    std::mutex m_InGameStateWatcherSwitch; /* 当 m_InGameStateWatcher 持有此对象时才能运行 */
    static void WatchGameProcess() noexcept; /* 监测游戏进程状态 */
    std::thread m_GameProcessWatcher; /* 监测游戏进程状态的线程对象 */
    std::mutex m_GameProcessWatcherSwitch; /* 当 m_GameProcessWatcher 持有此对象时才能运行 */
    CSOL_Utilities::CMessenger m_Messenger; /* 向 Lua Executor 下达命令 */
    HANDLE m_hGameProcess = nullptr; /* 游戏进程句柄 */
    HANDLE m_hGameWindow = nullptr; /* 游戏窗口句柄 */
};