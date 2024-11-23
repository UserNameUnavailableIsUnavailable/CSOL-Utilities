#pragma once

#include "CSOL_Utilities.hpp"
#include <string_view>

namespace CSOL_Utilities
{
    enum class COMMAND_DESCRIPTOR
    {
        COMMAND_NOP,
        COMMAND_START_GAME_ROOM,
        COMMAND_CHOOSE_CLASS,
        COMMAND_PLAY_GAME_NORMAL,
        COMMAND_PLAY_GAME_EXTEND,
        COMMAND_TRY_CONFIRM_RESULT,
        COMMAND_CREATE_GAME_ROOM,
        COMMAND_COMBINE_PARTS,
        COMMAND_PURCHASE_ITEM,
        COMMAND_LOCATE_CURSOR,
    };
    struct Command
    {
        COMMAND_DESCRIPTOR command_descriptor;
        std::time_t command_time;
    };
    constexpr std::string_view LookUpCommandString(COMMAND_DESCRIPTOR cmd)
    {
        switch (cmd)
        {
        case COMMAND_DESCRIPTOR::COMMAND_NOP:
            return u8"Command.COMMAND_NOP";
        case COMMAND_DESCRIPTOR::COMMAND_START_GAME_ROOM:
            return u8"Command.COMMAND_START_GAME_ROOM";
        case COMMAND_DESCRIPTOR::COMMAND_CHOOSE_CLASS:
            return u8"Command.COMMAND_CHOOSE_CLASS";
        case COMMAND_DESCRIPTOR::COMMAND_PLAY_GAME_NORMAL:
            return u8"Command.COMMAND_PLAY_GAME_NORMAL";
        case COMMAND_DESCRIPTOR::COMMAND_PLAY_GAME_EXTEND:
            return u8"Command.COMMAND_PLAY_GAME_EXTEND";
        case COMMAND_DESCRIPTOR::COMMAND_TRY_CONFIRM_RESULT:
            return u8"Command.COMMAND_TRY_CONFIRM_RESULT";
        case COMMAND_DESCRIPTOR::COMMAND_CREATE_GAME_ROOM:
            return u8"Command.COMMAND_CREATE_ROOM";
        case COMMAND_DESCRIPTOR::COMMAND_COMBINE_PARTS:
            return u8"Command.COMMAND_COMBINE_PARTS";
        case COMMAND_DESCRIPTOR::COMMAND_PURCHASE_ITEM:
            return u8"Command.COMMAND_PURCHASE_ITEM";
        case COMMAND_DESCRIPTOR::COMMAND_LOCATE_CURSOR:
            return u8"Command.COMMAND_LOCATE_CURSOR";
        default:
            return u8"Command.NOP";
        }
    }
}

namespace CSOL_Utilities
{
    struct AnalyzerOption
    {
        bool bActivate; /* 是否激活该模块 */
        bool bEnableExtendedAutoPlayMode; /* 是否开启扩展挂机模式 */
    };
}
