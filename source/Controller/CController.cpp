#include "CController.hpp"
#include "CConsole.hpp"
#include "CException.hpp"
#include <Windows.h>
#include <ctime>
#include <errhandlingapi.h>
#include <exception>
#include <filesystem>
#include <libloaderapi.h>

using namespace CSOL_Utilities;

std::mutex CController::s_SingletonStateMutex{};
CController *CController::s_Instance{nullptr};

void CController::InitializeInstance(std::string game_root, std::string cmd)
{
    std::filesystem::path Executor(std::filesystem::current_path() / "Executor");
    if (!std::filesystem::exists(Executor) || !std::filesystem::is_directory(Executor))
    {
        throw CException("目录 %s 不存在。", Executor.u8string().c_str());
    }
    if (!s_Instance)
    {
        std::lock_guard<std::mutex> lock_guard(s_SingletonStateMutex); /* lock_guard 防止创建单例时的并发问题 */
        if (!s_Instance)
        {
            s_Instance = new CController(game_root, std::move(cmd));
        }
    }
}

CController &CController::RetrieveInstance()
{
    if (s_Instance)
        return *s_Instance;
    else
        throw CException("CController 未初始化。");
}

void CController::DestroyInstance() noexcept
{
    std::lock_guard<std::mutex> lock_guard(s_SingletonStateMutex);
    if (s_Instance)
    {
        delete s_Instance;
        s_Instance = nullptr;
    }
}

void CController::RunInstance()
{
    m_ThreadExitEvent.Wait(); /* 若有线程退出，解除阻塞状态 */
    if (!m_ExitThreads)
    { /* 退出命令未下达，则说明是异常退出 */
        throw CException("遇到错误，无法继续运行。");
    }
}

CController::CController(std::string game_root_path, std::string launch_game_cmd)
    : m_GameRootPath(game_root_path),
      /* 开始运行时为 0 模式，两个 watcher 线程和一个 dispatcher 线程都处于阻塞状态，需要将 finished
         置位，否则会导致热键处理线程死锁 */
      m_GameProcessWatcherFinished(true), m_InGameStateWatcherFinished(true), m_FixedCommandDispatcherFinished(true),
      m_LaunchGameCmd(ConvertUtf8ToUtf16(launch_game_cmd.c_str())),
      m_Messenger(std::filesystem::current_path() / "Executor" / "$~cmd.lua")
{
    long bias;
    _get_timezone(&bias);
    m_Bias = bias;
    m_hDllMod = LoadLibraryW(L".\\GamingTool.dll");
    if (!m_hDllMod)
    {
        throw CException("加载 GamingTool.dll 失败，错误代码：%lu。", GetLastError());
    }
    /* 线程在构造列表初始化完成后才进行创建（线程运行依赖于大量默认构造的互斥体、条件变量、事件） */
    m_HotKeyEventHandler = std::thread(HandleHotKeyEvent);
    m_GameProcessWatcher = std::thread(WatchGameProcess);
    m_InGameStateWatcher = std::thread(WatchInGameState);
    m_FixedCommandDispatcher = std::thread(DispatchFixedCommand, std::ref(m_Command));
}

CController::~CController() noexcept
{
    try
    {
        s_Instance->m_ExitThreads = true;
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
        FreeLibrary(m_hDllMod);
    }
    catch (std::exception &e)
    {
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, e.what());
    }
}
