#include "Controller.hpp"
#include <Windows.h>
#include <ctime>
#include <errhandlingapi.h>
#include <exception>
#include <filesystem>
#include <libloaderapi.h>
#include "Console.hpp"
#include "Exception.hpp"

using namespace CSOL_Utilities;

std::mutex Controller::s_SingletonStateMutex{};
Controller* Controller::s_Instance{nullptr};

void Controller::InitializeInstance(std::string game_root, std::string cmd)
{
	std::filesystem::path Lua_Executor(std::filesystem::current_path() / L"Executor");
	if (!std::filesystem::exists(Lua_Executor) || !std::filesystem::is_directory(Lua_Executor))
	{
		throw ControllerException("目录 %s 不存在。", Lua_Executor.u8string().c_str());
	}
	/* Double check */
	if (!s_Instance)
	{
		std::lock_guard<std::mutex> lock_guard(s_SingletonStateMutex); /* lock_guard 防止创建单例时的并发问题 */
		if (!s_Instance)
		{
			s_Instance = new Controller(game_root, std::move(cmd));
		}
	}
}

Controller& Controller::RetrieveInstance()
{
	if (s_Instance)
		return *s_Instance;
	else
		throw ControllerException("Controller 未初始化。");
}

void Controller::DestroyInstance() noexcept
{
	std::lock_guard<std::mutex> lock_guard(s_SingletonStateMutex);
	if (s_Instance)
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}

void Controller::RunInstance()
{
	m_ThreadExitEvent.Wait(); /* 若有线程退出，解除阻塞状态 */
	if (!m_ExitThreads)
	{ /* 退出命令未下达，则说明是异常退出 */
		throw ControllerException("遇到错误，无法继续运行。");
	}
}

Controller::Controller(std::string game_root_path, std::string launch_game_cmd, std::string lghub_agent_name) :
	m_GameRootPath(game_root_path),
	/* 开始运行时为 0 模式，两个 watcher 线程和一个 dispatcher 线程都处于阻塞状态，需要将 finished
	   置位，否则会导致热键处理线程死锁 */
	m_GameProcessWatcherFinished(true), m_InGameStateWatcherFinished(true), m_FixedCommandDispatcherFinished(true),
	m_LaunchGameCmd(ConvertUtf8ToUtf16(launch_game_cmd.c_str())),
	m_Messenger(std::filesystem::current_path() / L"Executor" / L"$~cmd.lua")
{
	long bias;
	_get_timezone(&bias);
	m_Bias = bias;
	m_hDllMod = LoadLibraryW(L"GamingTool.dll");
	if (!m_hDllMod)
	{
		throw ControllerException("加载 GamingTool.dll 失败，错误代码：%lu。", GetLastError());
	}
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	DWORD dwNumberOfThreads = 1; /* 经过测试，开多个线程并不能显著减少识别时间 */
	m_hOcr = OcrInit(DET_MODEL_FILE, CLS_MODEL_FILE, REC_MODEL_FILE, KEY_FILE, dwNumberOfThreads);
	if (!m_hOcr)
	{
		throw ControllerException("初始化 OCR 遇到致命错误。");
	}
	/* 线程在构造列表初始化完成后才进行创建（线程运行依赖于大量默认构造的互斥体、条件变量、事件） */
	m_HotKeyEventHandler = std::thread(HandleHotKeyEvent);
	m_GameProcessWatcher = std::thread(WatchGameProcess);
	m_InGameStateWatcher = std::thread(WatchInGameState);
	m_FixedCommandDispatcher = std::thread(DispatchFixedCommand, std::ref(m_PeriodicCommand));
}

Controller::~Controller() noexcept
{
	try
	{
		/* 特别注意各线程及资源之间的依赖关系，有序释放 */
		s_Instance->m_ExitThreads = true;
		auto hThread = static_cast<HANDLE>(m_HotKeyEventHandler.native_handle());
		PostThreadMessage(GetThreadId(hThread), WM_QUIT, 0, 0);
		m_HotKeyEventHandler.join();
		s_Instance->m_GameProcessWatcherSwitch.set();
		m_GameProcessWatcher.join();
		s_Instance->m_GameProcessAlive.set();
		s_Instance->m_InGameStateWatcherSwitch.set();
		m_InGameStateWatcher.join();
		s_Instance->m_FixedCommandDispatcherSwitch.set();
		m_FixedCommandDispatcher.join();
		OcrDestroy(m_hOcr);
		FreeLibrary(m_hDllMod);
	}
	catch (std::exception& e)
	{
		Console::Log(ERROR_LEVEL::CUML_ERROR, e.what());
	}
}
