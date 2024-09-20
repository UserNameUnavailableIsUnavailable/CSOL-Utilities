#include "CCommander.hpp"
#include "CException.hpp"
#include <ctime>
#include <filesystem>

using namespace CSOL_Utilities;

std::mutex CCommander::s_singletonStateMutex{ };
CCommander* CCommander::s_Instance{ nullptr };

inline void CCommander::InitializeInstance(std::string sGameRootPath, std::string sGameLauncherPath)
{
    if (!std::filesystem::exists("./Executor") || !std::filesystem::is_directory("./Executor"))
    {
        throw CException("目录 .\\Executor 不存在。");
    }
    if (!s_Instance)
    {
        std::lock_guard<std::mutex> lock_guard(s_singletonStateMutex); /* lock_guard 防止创建单例时的并发问题 */
        if (!s_Instance)
        {
            s_Instance = new CCommander(sGameRootPath, sGameLauncherPath);
        }
    }
}

CCommander& CCommander::RetrieveInstance()
{
    if (s_Instance)
        return *s_Instance;
    else throw CException("CCommander 未初始化。");
}

inline void CCommander::DestroyInstance() noexcept
{
    if (s_Instance)
    {
        std::lock_guard<std::mutex> lock_guard(s_singletonStateMutex);
        delete s_Instance;
        s_Instance = nullptr;
    }
}

CCommander::CCommander(std::string sGameRootPath, std::string sGameLaucherPath) :
m_GameRootPath(sGameRootPath),
m_GameLauncherPath(sGameLaucherPath),
m_Messenger("./Executor/$~cmd.lua"),
m_HotKeyEventHandler(HandleHotKeyEvent)
{
    long bias;
    _get_timezone(&bias);
    m_Bias = bias;
    m_InGameStateWatcher = std::move(std::thread(WatchInGameState, std::ref(s_Instance)));
    m_GameProcessWatcher = std::move(std::thread(WatchGameProcess));
}

CCommander::~CCommander() noexcept
{
    s_Instance->m_bExitThreads = true;
    m_HotKeyEventHandler.join();
    m_InGameStateWatcher.join();
    m_GameProcessWatcher.join();
}
