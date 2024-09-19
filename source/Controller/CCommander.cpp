#include "CCommander.hpp"
#include <filesystem>

using namespace CSOL_Utilities;

std::mutex CCommander::s_singletonStateMutex{ };
CCommander* CCommander::s_Controller{ nullptr };

inline void CCommander::InitializeInstance(std::string sGameRootPath, std::string sGameLauncherPath)
{
    if (!std::filesystem::exists("./Executor") || !std::filesystem::is_directory("./Executor"))
    {
        
    }
    if (!s_Controller)
    {
        std::lock_guard<std::mutex> lock_guard(s_singletonStateMutex); /* lock_guard 防止创建单例时的并发问题 */
        if (!s_Controller)
        {
            s_Controller = new CCommander(sGameRootPath, sGameLauncherPath);
        }
    }
}

inline CCommander& CCommander::RetrieveObject() noexcept
{
    return *s_Controller;
}

inline void CCommander::DestroyInstance() noexcept
{
    if (s_Controller)
    {
        std::lock_guard<std::mutex> lock_guard(s_singletonStateMutex);
        delete s_Controller;
        s_Controller = nullptr;
    }
}

CCommander::CCommander(std::string sGameRootPath, std::string sGameLaucherPath) :
m_GameRootPath(sGameRootPath),
m_GameLauncherPath(sGameLaucherPath),
m_Messenger("./Executor/$~cmd.lua"),
m_HotKeyHandler(HandleHotKeyEvent)
{
    m_InGameStateWatcherSwitch.lock();
    m_GameProcessWatcherSwitch.lock();
    m_InGameStateWatcher = std::move(std::thread(WatchInGameState));
    m_GameProcessWatcher = std::move(std::thread(WatchGameProcess));
}
