#pragma once
#include <mutex>

namespace CSOL_Utilities
{
	enum class COMMAND
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
        CMD_RESERVED
	};
    class Command
    {
    public:
        void set(COMMAND cmd, bool repeatable = false);
        void get(std::string& s);
        static constexpr const char* query_command_string(COMMAND cmd) noexcept
        {
            switch (cmd)
            {
            case COMMAND::CMD_NOP:
                return "Command.CMD_NOP";
            case COMMAND::CMD_START_GAME_ROOM:
                return "Command.CMD_START_GAME_ROOM";
            case COMMAND::CMD_CHOOSE_CHARACTER:
                return "Command.CMD_CHOOSE_CHARACTER";
            case COMMAND::CMD_DEFAULT_IDLE:
                return "Command.CMD_DEFAULT_IDLE";
            case COMMAND::CMD_EXTENDED_IDLE:
                return "Command.CMD_EXTENDED_IDLE";
            case COMMAND::CMD_CONFIRM_RESULTS:
                return "Command.CMD_CONFIRM_RESULTS";
            case COMMAND::CMD_CREATE_GAME_ROOM:
                return "Command.CMD_CREATE_GAME_ROOM";
            case COMMAND::CMD_BATCH_COMBINE_PARTS:
                return "Command.CMD_BATCH_COMBINE_PARTS";
            case COMMAND::CMD_BATCH_PURCHASE_ITEM:
                return "Command.CMD_BATCH_PURCHASE_ITEM";
            case COMMAND::CMD_LOCATE_CURSOR:
                return "Command.CMD_LOCATE_CURSOR";
            case COMMAND::CMD_CLEAR_POPUPS:
                return "Command.CMD_CLEAR_POPUPS";
            default:
                return "Command.NOP";
            }
        }
        void nop();
    private:
        std::mutex m_mtx;
        uint64_t m_id;
        COMMAND m_cmd;
        // std::time_t m_tp; /* 命令下达的时刻 */
        bool m_repeatable; /* 是否可重复 */
    };
}