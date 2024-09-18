#include "CController.hpp"
#include <fstream>

using namespace CSOL_Utilities;

std::mutex CController::s_singletonStateMutex{ };
CController* CController::s_controller{ nullptr };

inline void CController::InitializeInstance(std::string sGameRootPath, std::string sGameLauncherPath)
{
    if (!s_controller)
    {
        std::lock_guard<std::mutex> lock_guard(s_singletonStateMutex); /* lock_guard 防止创建单例时的并发问题 */
        if (!s_controller)
        {
            s_controller = new CController(sGameRootPath, sGameLauncherPath);
        }
    }
}

inline CController& CController::RetrieveObject() noexcept
{
    return *s_controller;
}

inline void CController::DestroyInstance() noexcept
{
    if (s_controller)
    {
        std::lock_guard<std::mutex> lock_guard(s_singletonStateMutex);
        delete s_controller;
        s_controller = nullptr;
    }
}

CController::CController(std::string sGameRootPath, std::string sGameLaucherPath) :
m_GameRootPath(sGameRootPath), m_GameLauncherPath(sGameLaucherPath)
{
    m_ErrLogFile = std::ifstream(m_GameRootPath/"Error.log", std::ios_base::in);
    m_CommandFile = std::ofstream("./Executor/$~cmd.lua", std::ios::out);
}