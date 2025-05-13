#include "pch.hpp"

#include "CommandDispatcher.hpp"
#include "Command.hpp"
#include "Console.hpp"
#include "Utilities.hpp"
#include "Exception.hpp"

namespace CSOL_Utilities
{
	void CommandDispatcher::Work(std::stop_token st)
	{
		while (true)
		{
			{
				std::unique_lock lk(m_StateLock);
				m_Runnable.wait(lk, [this, &st] {
					return st.stop_requested() || m_bRunnable;
				});
				if (st.stop_requested())
				{
					break;
				}
				m_bFinished = false;
			}
			DWORD dwBytesWritten;
			thread_local std::string last_cmd;
			Command::Get(m_content);
			#ifdef _DEBUG
			Console::Debug(std::format("命令：{}。", m_content));
			#endif
			if (m_content != last_cmd) /* 命令内容发生变更才写入 */
			{
				SetFilePointer(m_hFile.get(), 0, NULL, FILE_BEGIN); // 移动到文件开始处
				auto write_ok = WriteFile(m_hFile.get(), m_content.c_str(), m_content.length() * sizeof(char), &dwBytesWritten, NULL);
				if (write_ok)
				{
					// 这里不使用 filesystem 的 resize_file，在已经有 Win32 句柄的情况下直接设置 EOF，提高效率
					SetFilePointer(m_hFile.get(), dwBytesWritten, NULL, FILE_BEGIN);
					SetEndOfFile(m_hFile.get()); // 写入 EOF
				}
				else
				{
					Console::Warn(Translate("CommandDispatcher::ERROR_WriteFile@1", GetLastError()));
				}
				swap(last_cmd, m_content); /* 交换当前命令内容及上一次命令，避免重复创建新的字符串对象，提高效率 */
			}
			{
				std::lock_guard lk(m_StateLock);
				m_bFinished = true;
			}
			m_Finished.notify_one();
			SleepEx(4000, true);
		}
	}

	CommandDispatcher::CommandDispatcher(std::filesystem::path command_file_path) :
		m_file_path(command_file_path),
		m_hFile(nullptr, [] (HANDLE h) { if (h && h != INVALID_HANDLE_VALUE) { CloseHandle(h); } })
	{
		m_hFile.reset(CreateFileW(m_file_path.wstring().c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
							  FILE_ATTRIBUTE_HIDDEN, NULL));
		if (m_hFile.get() == INVALID_HANDLE_VALUE)
		{
			throw Exception(Translate("CommandDispatcher::ERROR_CreateFileW@2", ConvertUtf16ToUtf8(m_file_path.wstring()), GetLastError()));
		}
		m_Worker = std::thread(
			[this] (std::stop_token st) {
				std::string module_name = "CommandDispatcher";
				try {
					Work(st);
				} catch (std::exception& e) {
					Console::Error(Translate("Module::ERROR_ModulePanic@2", module_name, e.what()));
				}
				Console::Info(Translate("Module::INFO_ModuleExited@1", module_name));
			},
			m_StopSource.get_token()
		);
	}

    void CommandDispatcher::Suspend()
	{
		std::unique_lock lk(m_StateLock);
		m_bRunnable = false;
		if (m_StopSource.get_token().stop_requested())
		{
			return;
		}
		m_Finished.wait(lk, [this] { return m_bFinished; });
	}

	void CommandDispatcher::Resume()
	{
		{
			std::lock_guard lk(m_StateLock);
			m_bRunnable = true;
		}
		m_Runnable.notify_one();
	}

	CommandDispatcher::~CommandDispatcher() noexcept
	{
		m_StopSource.request_stop();
		m_Runnable.notify_one();
        QueueUserAPC([] (ULONG_PTR) {}, reinterpret_cast<HANDLE>(m_Worker.native_handle()), 0);
		m_Worker.join();
	}
} // namespace CSOL_Utilities
