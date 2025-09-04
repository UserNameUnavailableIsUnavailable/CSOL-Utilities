#pragma once

#include "Module.hpp"

namespace CSOL_Utilities
{
	class CommandDispatcher : public Module
	{
	public:
		CommandDispatcher(std::filesystem::path command_file_path);
		~CommandDispatcher() noexcept;
		virtual void Resume() override;
		virtual void Suspend() override;

	private:
		void WriteCommandFile(const std::string& command_string);
		void Work(std::stop_token st);
		std::stop_source m_StopSource;
        /* 考虑到 dangling resources 问题，这里使用 thread 而非 jthread */
		std::thread m_Worker;
		std::mutex m_StateLock;
		std::condition_variable m_Runnable;
		std::condition_variable m_Finished;
		bool m_bRunnable = false;
		bool m_bFinished = true;

		std::filesystem::path m_file_path; /* 用 filesystem 处理文件路径 */
		std::unique_ptr<std::remove_pointer_t<HANDLE>, void (*) (HANDLE)> m_hFile; /* 命令内容大小基本固定，直接采用 Win32 API 写命令以提高效率 */
	};

} // namespace CSOL_Utilities
