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
			thread_local std::string last_cmd;
			thread_local std::string this_cmd;
			Command::Get(this_cmd);
			WriteCommandFile(this_cmd);
			swap(last_cmd, this_cmd); /* 交换当前命令内容及上一次命令，避免重复创建新的字符串对象，提高效率 */
			{
				std::lock_guard lk(m_StateLock);
				m_bFinished = true;
			}
			m_Finished.notify_one();
			SleepEx(1000, true); /* 每隔 1 秒运行一次，但不一定会触发文件写入 */
		}
	}

	CommandDispatcher::CommandDispatcher(std::filesystem::path command_file_path) :
		m_file_path(command_file_path),
		m_hFile(INVALID_HANDLE_VALUE, [] (HANDLE h) { if (h && h != INVALID_HANDLE_VALUE) { CloseHandle(h); } })
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

	void CommandDispatcher::WriteCommandFile(std::string_view command_string)
	{
		DWORD dwBytesWritten = 0;
		auto write_ok = WriteFile(m_hFile.get(), command_string.data(), command_string.length() * sizeof(char), &dwBytesWritten, NULL);
		if (write_ok)
		{
			SetFilePointer(m_hFile.get(), dwBytesWritten, NULL, FILE_BEGIN);
			SetEndOfFile(m_hFile.get());
		}
		else
		{
			Console::Warn(Translate("CommandDispatcher::ERROR_WriteFile@1", GetLastError()));
		}
	}

	CommandDispatcher::~CommandDispatcher() noexcept
	{
		m_StopSource.request_stop();
		m_Runnable.notify_one();
        QueueUserAPC([] (ULONG_PTR) {}, reinterpret_cast<HANDLE>(m_Worker.native_handle()), 0);
		m_Worker.join();
		WriteCommandFile(Command::NOP()); /* 确保最后写入 NOP */
	}
} // namespace CSOL_Utilities
