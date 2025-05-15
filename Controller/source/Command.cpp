#include "pch.hpp"

#include "Command.hpp"
#include "Utilities.hpp"
#include "Exception.hpp"

namespace CSOL_Utilities
{
	void Command::Get(std::string& s)
	{
		auto& self = GetInstance();
		auto expected = false;

		while (!self.m_SpinLock.compare_exchange_strong(expected, true, std::memory_order_seq_cst)) {}

		if (self.m_AutoRenew) // 自动续期
		{
			auto tp = std::chrono::system_clock::now();
			if (tp - self.m_Timepoint > std::chrono::milliseconds(4000)) // 超过 4 秒触发续期
			{
				self.m_Timepoint = tp;
			}
		}
		auto new_size =
			std::formatted_size(COMMAND_FORMAT, self.m_Id, QueryCommandString(self.m_CmdType),
								static_cast<int64_t>(std::chrono::system_clock::to_time_t(self.m_Timepoint)), self.m_Repeatable);
		s.resize(new_size);
		std::format_to(s.data(), COMMAND_FORMAT, self.m_Id, QueryCommandString(self.m_CmdType),
					   static_cast<int64_t>(std::chrono::system_clock::to_time_t(self.m_Timepoint)), self.m_Repeatable);
		self.m_SpinLock.store(true, std::memory_order_release);
	}

	std::string Command::Get()
	{
		std::string ret;
		Get(ret);
		return ret;
	}

	void Command::Set(Command::TYPE cmd_type, Command::MODE mode)
	{
		auto& self = GetInstance();
		auto expected = false;

		while (!self.m_SpinLock.compare_exchange_strong(expected, true, std::memory_order_seq_cst)) {} // 自旋锁

		auto now = std::chrono::system_clock::now();

		if (self.m_CmdType == cmd_type && /* 连续下达同一命令 */
			std::chrono::duration_cast<std::chrono::milliseconds>(now - self.m_Timepoint) < QueryCommandCoolDownTime(cmd_type))
		{
			return; /* 冷却时间尚未结束 */
		}

		self.m_Id++;
		self.m_CmdType = cmd_type;
		self.m_Timepoint = static_cast<bool>(mode & CMD_ZERO_TIMESTAMP) ? std::chrono::time_point<std::chrono::system_clock>() : std::chrono::system_clock::now() ;
		self.m_Repeatable = static_cast<bool>(mode & CMD_REPEATABLE);
		self.m_AutoRenew = static_cast<bool>(mode & CMD_AUTO_REFRESH);
		
		self.m_SpinLock.store(false, std::memory_order_release); // 解锁
	}

	Command& Command::GetInstance()
	{
		static Command executor_command; // Meyer's singleton implementation
		return executor_command;
	}
} // namespace CSOL_Utilities
