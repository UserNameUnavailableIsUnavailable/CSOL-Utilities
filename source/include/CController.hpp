#pragma once
#include "CSOL_Utilities.hpp"
#include <Windows.h>
#include <thread>
#include <mutex>
#include <filesystem>
#include <fstream>

/*C
CCSOLUtilities 类，单例模式。
C*/
class CSOL_Utilities::CController
{
public:
    static inline void InitializeInstance(std::string sGameRootPath, std::string sGameLauncherPath);
    static inline CController& RetrieveObject() noexcept;
    static inline void DestroyInstance() noexcept;
private:
    CController(std::string sGameRootPath, std::string sGameLauncherPath);
    ~CController() noexcept;
    CController(const CController&) = delete;
    CController(const CController&&) = delete;
    CController& operator=(CController&) = delete;
    CController& operator=(CController&&) = delete;
private:
    static CController* s_controller;
    /* 状态标志 */
    static std::mutex s_singletonStateMutex;
    bool m_bExitThreads = false;
    std::filesystem::path m_GameRootPath; /* 游戏根目录 */
    std::filesystem::path m_GameLauncherPath; /* 启动游戏命令行 */
    /* 状态机所需资源 */
    std::ifstream m_ErrLogFile; /* 日志文件 */
    std::ofstream m_CommandFile; /* 命令文件，用于向 Lua Executor 下达命令 */
    /* 挂机 */
    static void WatchInGameState() noexcept; /* 监测游戏内状态 */
    std::thread m_InGameStateWatcher; /* 监测游戏内状态的线程对象 */
    std::mutex m_InGameStateWatcherSwitch; /* 当 m_InGameStateWatcher 持有此对象时才能运行 */
    static void WatchGameProcess() noexcept; /* 监测游戏进程状态 */
    std::thread m_GameProcessWatcher; /* 监测游戏进程状态的线程对象 */
    std::mutex m_GameProcessWatcherSwitch; /* 当 m_GameProcessWatcher 持有此对象时才能运行 */
    static void DispatchCommand() noexcept; /* 向 Lua Executor 下达命令 */
    std::thread m_CommandDispacher; /* 下达命令的线程对象 */
    std::mutex m_CommandDispatcherSwitch; /* 当 m_CommandDispacher 持有此对象时才能运行 */
    HANDLE m_hGameProcess = nullptr; /* 游戏进程句柄 */
    HANDLE m_hGameWindow = nullptr; /* 游戏窗口句柄 */
};