#pragma once
#include <filesystem>
#include <fstream>

/*C
CCSOLUtilitiy 类，单例模式。
C*/
class CCSOLUtilitiy
{
public:
    static inline void InitializeInstance();
    static inline void DestroyInstance() noexcept;
private:
    CCSOLUtilitiy(std::string sGameRootPath, std::string sGameLauncherPath);
    ~CCSOLUtilitiy() noexcept;
    CCSOLUtilitiy(const CCSOLUtilitiy&) = delete;
    CCSOLUtilitiy(const CCSOLUtilitiy&&) = delete;
    CCSOLUtilitiy& operator=(CCSOLUtilitiy&) = delete;
    CCSOLUtilitiy& operator=(CCSOLUtilitiy&&) = delete;
private:
    static CCSOLUtilitiy* m_singleton;
    /* 状态标志 */
    bool m_bHasInitialized = false;
    bool m_bBeingDestroyed = false;
    std::mutex m_stateMutex;
    std::filesystem::path m_sGameRootPath; /* 游戏根目录 */
    std::filesystem::path m_sGameLauncherPath; /* 启动游戏命令行 */
    /* 状态机所需资源 */
    std::fstream m_errLogFile; /* 日志文件 */
    std::fstream m_CommandFile; /* 命令文件，用于向 Lua Executor 下达命令 */
    /* 挂机 */

};