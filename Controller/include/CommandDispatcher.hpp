#pragma once

#include "Module.hpp"

namespace CSOL_Utilities
{
	class CommandDispatcher : public Module
	{
	public:
		CommandDispatcher(std::filesystem::path command_file_path);
		~CommandDispatcher() noexcept;
		virtual void Boot() override;
		virtual void Resume() noexcept override;
		virtual void Suspend() noexcept override;
		virtual void Terminate() noexcept override;

	private:
		std::mutex boot_lock_;
		bool booted_ = false;

		void WriteCommandFile(const std::string& command_string);
		void Run(std::stop_token st);
		std::stop_source stop_source_;
		/* 考虑到 dangling resources 问题，这里使用 thread 而非 jthread */
		std::thread worker_;
		std::mutex worker_state_lock_;
		std::condition_variable worker_runnable_cond_;
		std::condition_variable worker_finished_cond_;
		bool is_worker_runnable_ = false;
		bool has_worker_finished_ = true;

		std::filesystem::path cmd_file_path_; /* 用 filesystem 处理文件路径 */
		std::unique_ptr<std::remove_pointer_t<HANDLE>, void (*)(HANDLE)>
			file_handle; /* 命令内容大小基本固定，直接采用 Win32 API 写命令以提高效率 */
	};

} // namespace CSOL_Utilities
