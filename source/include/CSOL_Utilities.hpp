#pragma once

namespace CSOL_Utilities
{
    class CCommander;
    class CMessenger;
    enum COMMAND
    {
        NOP,
        START_GAME_ROOM,
        CHOOSE_CLASS,
        PLAY_GAME_NORMAL,
        PLAY_GAME_EXTEND,
        TRY_CONFIRM_RESULT,
        CREATE_ROOM,
        COMBINE_PARTS,
        PURCHASE_ITEM,
        LOCATE_CURSOR,
    };
    class CException;
}