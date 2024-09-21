#include "CController.hpp"
#include "CConsole.hpp"
#include "CException.hpp"
#include <Windows.h>
#include <ctime>
#include <exception>
#include <filesystem>

using namespace CSOL_Utilities;

std::mutex CController::s_singletonStateMutex{ };
CController* CController::s_Instance{ nullptr };

void CController::InitializeInstance(std::string sGameRootPath, std::string sGameLauncherPath)
{
    std::filesystem::path Executor(std::filesystem::current_path()/"Executor");
    if (!std::filesystem::exists(Executor) || !std::filesystem::is_directory(Executor))
    {
        throw CException("目录 %s 不存在。", Executor.u8string().c_str());
    }
    if (!s_Instance)
    {
        std::lock_guard<std::mutex> lock_guard(s_singletonStateMutex); /* lock_guard 防止创建单例时的并发问题 */
        if (!s_Instance)
        {
            s_Instance = new CController(sGameRootPath, sGameLauncherPath);
        }
    }
}

CController& CController::RetrieveInstance()
{
    if (s_Instance)
        return *s_Instance;
    else throw CException("CController 未初始化。");
}

void CController::DestroyInstance() noexcept
{
    if (s_Instance)
    {
        std::lock_guard<std::mutex> lock_guard(s_singletonStateMutex);
        delete s_Instance;
        s_Instance = nullptr;
    }
}

void CController::RunInstance()
{
    m_bThreadExit.Wait(); /* 若有线程退出，解除阻塞状态 */
    if (!m_bExitThreads) { /* 退出命令未下达，则说明是异常退出 */
        throw CException("遇到错误，无法继续运行。");
    }
}

CController::CController(std::string sGameRootPath, std::string sGameLaucherPath) :
m_GameRootPath(sGameRootPath),
m_GameLauncherPath(sGameLaucherPath),
m_Messenger(std::filesystem::current_path()/"Executor"/"$~cmd.lua")
{
    long bias;
    _get_timezone(&bias);
    m_Bias = bias;
    /* 线程在构造列表初始化完成后才进行创建（线程运行依赖于大量默认构造的互斥体、条件变量、事件） */
    // m_HotKeyEventHandler = std::thread(HandleHotKeyEvent);
    m_InGameStateWatcher = std::thread(WatchInGameState);
    m_GameProcessWatcher = std::thread(WatchGameProcess);
    m_FixedCommandDispatcher = std::thread(DispatchFixedCommand, std::ref(m_Command));
}

CController::~CController() noexcept
{
    s_Instance->m_bExitThreads = true;
    auto hThread = static_cast<HANDLE>(m_HotKeyEventHandler.native_handle());
    PostThreadMessage(GetThreadId(hThread), WM_QUIT, 0, 0);
    m_HotKeyEventHandler.join();
    s_Instance->m_GameProcessWatcherSwitch.Set();
    m_GameProcessWatcher.join();
    s_Instance->m_GameProcessAlive.Set();
    s_Instance->m_InGameStateWatcherSwitch.Set();
    m_InGameStateWatcher.join();
    s_Instance->m_FixedCommandDispatcherSwitch.Set();
    m_FixedCommandDispatcher.join();
}
