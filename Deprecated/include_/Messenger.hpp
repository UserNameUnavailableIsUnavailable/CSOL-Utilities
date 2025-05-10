#pragma once

#include <filesystem>
#include <fstream>
#include <mutex>
#include "CSOL_Utilities.hpp"
#include "Command.hpp"
#include "ExecutorCommand.hpp"

namespace CSOL_Utilities
{
	class Messenger
	{
	public:
		explicit Messenger(std::filesystem::path file);
		Messenger(const Messenger&) = delete;
		Messenger& operator=(const Messenger&) = delete;
		Messenger(const Messenger&&) = delete;
		Messenger& operator=(const Messenger&&) = delete;
		~Messenger() noexcept;
		void Dispatch(const ExecutorCommand& ec) noexcept;
		void DispatchNOP() noexcept;

	private:
		static constexpr const char* QueryCommandString(COMMAND cmd) noexcept;
		std::filesystem::path m_CommandFile;
		std::ofstream m_FileStream;
		const char* m_CommandString;
		std::time_t m_CommandTimepoint{0};
		std::mutex m_Mutex;
		char* m_Buffer;
	};
}; // namespace CSOL_Utilities
