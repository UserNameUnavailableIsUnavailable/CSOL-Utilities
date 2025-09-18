﻿#pragma once

namespace CSOL_Utilities
{
	class Command
	{
	public:
		enum class TYPE
		{
			CMD_NOP,
			CMD_START_GAME_ROOM,
			CMD_CHOOSE_CHARACTER,
			CMD_DEFAULT_IDLE,
			CMD_EXTENDED_IDLE,
			CMD_CONFIRM_RESULTS,
			CMD_CREATE_GAME_ROOM,
			CMD_BATCH_COMBINE_PARTS,
			CMD_BATCH_PURCHASE_ITEM,
			CMD_LOCATE_CURSOR,
			CMD_CLEAR_POPUPS,
			CMD_DETECT_IN_GAME,
			CMD_DEFAULT_IDLE_2,
			CMD_EXTENDED_IDLE_2,
		};

		using MODE = unsigned int;
		static constexpr const MODE CMD_DEFAULT = 0;
		static constexpr const MODE CMD_REPEATABLE = 1 << 1;
		static constexpr const MODE CMD_AUTO_REFRESH = 1 << 2;
		static constexpr const MODE CMD_ZERO_TIMESTAMP = 1 << 3;

		static void Set(Command::TYPE cmd_type, Command::MODE mode = Command::CMD_DEFAULT);
		static std::string Get();
		static void Get(std::string& s);
		static constexpr std::string_view NOP() { return "Command.directives={id=0,type=Command.CMD_NOP,timepoint=0,repeatable=false}"; }
		static constexpr std::string_view QueryCommandString(Command::TYPE cmd_type) noexcept
		{
			switch (cmd_type)
			{
			case Command::TYPE::CMD_NOP:
				return "Command.CMD_NOP";
			case Command::TYPE::CMD_START_GAME_ROOM:
				return "Command.CMD_START_GAME_ROOM";
			case Command::TYPE::CMD_CHOOSE_CHARACTER:
				return "Command.CMD_CHOOSE_CHARACTER";
			case Command::TYPE::CMD_DEFAULT_IDLE:
				return "Command.CMD_DEFAULT_IDLE";
			case Command::TYPE::CMD_EXTENDED_IDLE:
				return "Command.CMD_EXTENDED_IDLE";
			case Command::TYPE::CMD_CONFIRM_RESULTS:
				return "Command.CMD_CONFIRM_RESULTS";
			case Command::TYPE::CMD_CREATE_GAME_ROOM:
				return "Command.CMD_CREATE_GAME_ROOM";
			case Command::TYPE::CMD_BATCH_COMBINE_PARTS:
				return "Command.CMD_BATCH_COMBINE_PARTS";
			case Command::TYPE::CMD_BATCH_PURCHASE_ITEM:
				return "Command.CMD_BATCH_PURCHASE_ITEM";
			case Command::TYPE::CMD_LOCATE_CURSOR:
				return "Command.CMD_LOCATE_CURSOR";
			case Command::TYPE::CMD_CLEAR_POPUPS:
				return "Command.CMD_CLEAR_POPUPS";
			case Command::TYPE::CMD_DETECT_IN_GAME:
				return "Command.CMD_DETECT_IN_GAME";
			case Command::TYPE::CMD_DEFAULT_IDLE_2:
				return "Command.CMD_DEFAULT_IDLE_2";
			case Command::TYPE::CMD_EXTENDED_IDLE_2:
				return "Command.CMD_EXTENDED_IDLE_2";
			default:
				return "Command.NOP";
			}
		}
		static constexpr std::chrono::milliseconds QueryCommandCoolDownTime(Command::TYPE cmd)
		{
			switch (cmd)
			{
			/* 创建一个房间的耗时在 15 秒左右，进行一次状态判定的耗时从 1 ~ 6 秒不等，具体取决于硬件配置 */
			/* 假定某一段时间内状态判定耗时均为 τ 秒，那么为了使申请到新 ID 时的判定的状态是准确的，则： */
			/* 申请分配新命令 ID 的间隔时间应为 15 + τ */
			/* 保守起见，这里将冷却时间设置为 30 秒 */
			case Command::TYPE::CMD_CREATE_GAME_ROOM:
				return std::chrono::seconds(30);
			default:
				return std::chrono::milliseconds(4000);
			}
		}

	private:
		static Command& GetInstance();
        Command() = default;
		~Command() noexcept = default;
		Command(Command&) = delete;
		Command(Command&&) = delete;

		std::atomic_bool m_SpinLock;
		
		TYPE type_ = TYPE::CMD_NOP;
		uint64_t id_ = 0;
		bool repeatable_ = false; /* 是否可重复 */
		bool auto_renew_ = false; /* 自动刷新 */
		std::chrono::time_point<std::chrono::system_clock> timepoint_ = std::chrono::system_clock::time_point(); /* 命令下达时刻 */
		static constexpr std::string_view DIRECTIVES_TEMPLATE = "Command.directives={{id={}, type={},timepoint={},repeatable={}}}"; /* 指令模板 */
	};
} // namespace CSOL_Utilities
