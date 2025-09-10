#include "IdleEngine.hpp"
#include "Console.hpp"
#include "Global.hpp"
#include "Utilities.hpp"

using namespace CSOL_Utilities;

IdleEngine::IdleEngine(std::unique_ptr<GameProcessInformation> game_process_info) :
	game_process_info_(std::move(game_process_info))
{
}

void IdleEngine::Boot()
{
	std::lock_guard lk(boot_lock_);
	if (is_booted_)
		return; // 防止重复启动
	process_watcher_ = std::thread(
		[this](std::stop_token st)
		{
			std::string module_name = "ProcessDetector";
			while (true)
			{
				bool has_error_occurred = false;
				/* 细化封锁粒度，基于作用域解锁，避免死锁 */
				{
					std::unique_lock lk(threads_state_lock_);
					process_watcher_runnable_.wait(lk, [this, &st]
												   { return st.stop_requested() || is_process_watcher_runnable_; });
					if (st.stop_requested()) /* 退出 */
					{
						break;
					}
					has_process_watcher_finished_ = false;
				}
				try
				{
					Watch();
				}
				catch (std::exception& e)
				{
					has_error_occurred = true;
					Console::Error(Translate("Module::ERROR_ModulePanic@2", module_name, e.what()));
				}
				// finally
				{
					std::lock_guard lk(threads_state_lock_);
					has_process_watcher_finished_ = true;
				}
				process_watcher_finished_.notify_one();
				if (has_error_occurred)
				{
					break;
				}
				else
				{
					SleepEx(GetWatchdogInterval(), true);
				}
			}
			Console::Info(Translate("Module::INFO_ModuleExited@1", module_name));
			return 0;
		},
		stop_source_.get_token());
	scene_discriminator_ = std::thread(
		[this](std::stop_token st)
		{
			std::string module_name = "SceneDiscriminator";
			while (true)
			{
				bool has_error_occurred = false;
				/* 细化封锁粒度，基于作用域解锁，避免死锁 */
				{
					std::unique_lock lk(threads_state_lock_);
					scene_discriminator_runnable_.wait(lk,
													   [this, &st]
													   {
														   return st.stop_requested() ||
															   (is_process_watcher_runnable_ &&
																is_scene_discriminator_runnable_);
													   });
					if (st.stop_requested())
					{
						break;
					}
                    has_scene_discriminator_finished_ = false;
				}
				try
				{
					Discriminate();
				}
				catch (std::exception& e)
				{
					has_error_occurred = true;
					Console::Error(Translate("Module::ERROR_ModulePanic@2", module_name, e.what()));
				}
				// finally
				{
					std::lock_guard lk(threads_state_lock_);
					has_scene_discriminator_finished_ = true;
				}
				scene_discriminator_finished_.notify_one();
				if (has_error_occurred)
				{
					break;
				}
				else
				{
					SleepEx(GetDiscriminationInterval(), true);
				}
			}
			Console::Info(Translate("Module::INFO_ModuleExited@1", module_name));
			return 0;
		},
		stop_source_.get_token());
	is_booted_ = true; // 标记为启动
}

void IdleEngine::Terminate() noexcept
{
	std::lock_guard lk(boot_lock_);
	if (!is_booted_)
		return; // 未启动
	stop_source_.request_stop();
	process_watcher_runnable_.notify_one();
	scene_discriminator_runnable_.notify_one();
	// 子线程若使用 SleepEx 等待，则可以使用 APC 提前唤醒，而无需等待阻塞时间到达
	QueueUserAPC([](ULONG_PTR) {}, reinterpret_cast<HANDLE>(process_watcher_.native_handle()), 0);
	QueueUserAPC([](ULONG_PTR) {}, reinterpret_cast<HANDLE>(scene_discriminator_.native_handle()), 0);
	if (process_watcher_.joinable())
		process_watcher_.join();
	if (scene_discriminator_.joinable())
		scene_discriminator_.join();
	is_booted_ = false; // 标记为未启动
}

IdleEngine::~IdleEngine() noexcept
{
	Terminate();
}

void IdleEngine::Resume() noexcept
{
	{
		std::lock_guard lk(threads_state_lock_);
		is_process_watcher_runnable_ = true;
	}
	process_watcher_runnable_.notify_one();
	scene_discriminator_runnable_.notify_one();
}

void IdleEngine::Suspend() noexcept
{
	std::unique_lock lk(threads_state_lock_);
	is_process_watcher_runnable_ = false;
	if (stop_source_.get_token().stop_requested())
	{
		return;
	}
	if (has_process_watcher_finished_ &&
		has_scene_discriminator_finished_) /* 如果任务都已经执行完毕，则直接返回，减小开销 */
	{
		return;
	}
	QueueUserAPC([](ULONG_PTR) {}, reinterpret_cast<HANDLE>(process_watcher_.native_handle()),
				 0); /* 检测器可能正在等待进程，需要使用 APC 将其唤醒 */
	/* 阻塞等待两个线程完成剩余任务 */
	process_watcher_finished_.wait(lk, [this] { return has_process_watcher_finished_; });
	scene_discriminator_finished_.wait(lk, [this] { return has_scene_discriminator_finished_; });
}

void IdleEngine::Watch()
{
	if (process_state_ == GAME_PROCESS_STATE::RUNNING) // 游戏进程正在运行
	{
		DWORD dwStatus = WaitForSingleObjectEx(game_process_info_->get_process_handle(), INFINITE,
											   TRUE); /* 等待进程退出，或新的 APC 出现 */
		if (dwStatus == WAIT_OBJECT_0) /* 游戏进程退出 */
		{
			std::unique_lock lk(threads_state_lock_);
			is_scene_discriminator_runnable_ = false; /* 停止判别器运行 */
			scene_discriminator_finished_.wait(
				lk, [this] { return has_scene_discriminator_finished_; }); /* 等待判别器当前轮次执行完毕 */
			game_process_info_->clear(); /* 清除进程信息 */
			process_state_ = GAME_PROCESS_STATE::EXITED;
			Console::Warn(Translate("IdleEngine::GameProcessDetector::WARN_GameProcessExited"));
		}
	}
	else if (process_state_ == GAME_PROCESS_STATE::BEING_CREATED) // 游戏进程正在创建
	{
		assert(game_process_info_->get_process_handle() == nullptr);
		if (SearchGameWindow()) // 查找游戏窗口成功
		{
			SleepEx(10000, TRUE); // 等待 10 秒，确保游戏进程完全启动
			ResetAfterReconnection(); // 游戏启动，进入登陆状态，重置状态机
			Console::Info(Translate("IdleEngine::GameProcessDetector::INFO_ProcessDetected@1",
									game_process_info_->get_process_id()));
			if (GetIdleMode() == IDLE_MODE::EXTENDED &&
				Global::DefaultIdleAfterReconnection) // 掉线重连后切换到默认模式
			{
				idle_mode_.store(IDLE_MODE::DEFAULT, std::memory_order_release);
				Console::Info(Translate("IdleEngine::GameProcessDetector::INFO_SwitchToDefaultIdleMode"));
			}
			{
				std::lock_guard lk(threads_state_lock_);
				is_scene_discriminator_runnable_ = true;
			}
			scene_discriminator_runnable_.notify_one(); // 唤醒判别器线程
			process_state_ = GAME_PROCESS_STATE::RUNNING;
		}
	}
	else if (process_state_ == GAME_PROCESS_STATE::EXITED) // 游戏进程退出，需要重启
	{
		PROCESS_INFORMATION pi;
		STARTUPINFOW si{.cb = sizeof(STARTUPINFOW)};
		std::wstring launch_command(game_process_info_->get_launch_command());
		BOOL bRet = CreateProcessW(nullptr, launch_command.data(), nullptr, nullptr, false, NORMAL_PRIORITY_CLASS,
								   nullptr, L"C:\\WINDOWS\\SYSTEM32", &si, &pi);
		// pi.hProcess, pi.hThread 是游戏启动器进程的句柄，直接关闭
		if (bRet)
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else
		{
			Console::Warn(Translate("IdleEngine::GameProcessDetector::WARN_CreateProcessW@1", GetLastError()));
		}
		// 创建成功后等待游戏进程启动
		process_state_ = GAME_PROCESS_STATE::BEING_CREATED;
		Console::Info(Translate("IdleEngine::GameProcessDetector::INFO_WaitGameProcessLaunch"));
	}
	else if (process_state_ == GAME_PROCESS_STATE::UNKNOWN) // 初始状态未知
	{
		assert(game_process_info_->get_process_handle() == nullptr);
		if (SearchGameWindow()) // 查找游戏窗口
		{
			Console::Info(Translate("IdleEngine::GameProcessDetector::INFO_ProcessDetected@1",
									game_process_info_->get_process_id()));
			process_state_ = GAME_PROCESS_STATE::RUNNING;
			{
				std::lock_guard lk(threads_state_lock_);
				is_scene_discriminator_runnable_ = true;
			}
			scene_discriminator_runnable_.notify_one(); // 唤醒判别器线程
		}
		else // 游戏进程未运行
		{
			process_state_ = GAME_PROCESS_STATE::EXITED;
			Console::Info(Translate("IdleEngine::GameProcessDetector::INFO_GameProcessNotRunning"));
		}
	}
}

bool IdleEngine::SearchGameWindow()
{
	struct WindowInformation
	{
		const std::filesystem::path executable_path; // 可执行文件路径
		const std::wstring window_title; // 待查找的窗口标题
		bool found; // 是否找到
		DWORD process_id; // 查到的进程 ID
		HWND window_handle; // 查到的窗口句柄
		HANDLE process_handle; // 查到的进程句柄
	};
	WindowInformation params{
		.executable_path = game_process_info_->get_executable_path(),
		.window_title = game_process_info_->get_window_title(),
		.found = false,
		.process_id = 0,
		.window_handle = nullptr,
	};
	auto impl = [](HWND hWnd, LPARAM lParam) -> BOOL
	{
		auto params = reinterpret_cast<WindowInformation*>(lParam);

		std::wstring window_title;

		DWORD_PTR window_title_length = 0;
		// 向窗口发送 WM_GETTEXTLENGTH 消息，获取窗口标题长度，需要设置超时时间，防止有部分窗口不响应该消息
		auto ret = SendMessageTimeoutW(hWnd, WM_GETTEXTLENGTH, 0, 0, SMTO_ABORTIFHUNG, 200, &window_title_length);
		if (ret == 0) // 失败或超时
		{
#ifdef _DEBUG
			Console::Debug(std::format("枚举窗口（句柄：0x{:X}）失败或超时。", reinterpret_cast<std::uintptr_t>(hWnd)));
#endif
			return TRUE; // 继续枚举下一个窗口
		}
		if (window_title_length != params->window_title.length()) // 长度不匹配，直接跳过，减轻开销
		{
			return TRUE; // 继续枚举下一个窗口
		}
		window_title.resize(window_title_length + 1); // length 不包含 '\0'
		auto iSize = GetWindowTextW(hWnd, window_title.data(), static_cast<int>(window_title.size()));
		window_title.resize(iSize); // iSize 为不含末尾空字符的实际长度
#ifdef _DEBUG
		Console::Debug(std::format("窗口句柄：0x{:#x}，窗口标题：{}", reinterpret_cast<std::uintptr_t>(hWnd),
								   ConvertUtf16ToUtf8(window_title)));
#endif
		// 匹配窗口标题
		if (window_title == params->window_title)
		{
			params->window_handle = hWnd;
			// 获取窗口所属进程 ID
			DWORD dwOwnerProcessId = 0;
			GetWindowThreadProcessId(hWnd, &dwOwnerProcessId);
			// 获取进程的可执行文件路径
			if (dwOwnerProcessId == 0) // 无法获取进程 ID
			{
				return TRUE; // 继续枚举
			}
			// 打开进程
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, dwOwnerProcessId);
			if (!hProcess) // 无法打开进程
			{
				return TRUE; // 继续枚举
			}
			// 获取进程的可执行文件路径
			std::filesystem::path p;
			try
			{
				p = GetProcessImagePath(reinterpret_cast<uintptr_t>(hProcess));
			}
			catch (std::exception& e)
			{
#ifdef _DEBUG
				Console::Debug(std::format("获取进程（PID：{}）路径失败，错误信息：{}", dwOwnerProcessId, e.what()));
#endif
				return TRUE; // 继续枚举
			}
			// 检查路径是否匹配
			if (std::filesystem::equivalent(p, params->executable_path))
			{
				params->found = true;
				params->process_id = dwOwnerProcessId;
				params->process_handle = hProcess;
				return FALSE; // 停止枚举
			}
			else
			{
				CloseHandle(hProcess);
			}
		}
		return TRUE; // 继续枚举
	};
	EnumWindows(impl, reinterpret_cast<LPARAM>(&params));
	// 更新窗口句柄
	if (params.found)
	{
		game_process_info_->set_window_handle(params.window_handle);
		game_process_info_->set_process_id(params.process_id);
		game_process_info_->set_process_handle(params.process_handle);
	}
	else
	{
		game_process_info_->set_window_handle(nullptr);
		game_process_info_->set_process_id(0);
		game_process_info_->set_process_handle(nullptr);
	}
	return params.found;
}
