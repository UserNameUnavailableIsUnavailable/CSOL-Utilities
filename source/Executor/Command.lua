if (not Command_lua)
then
Command_lua = true
Command = {
    CMD_NOP = 0,
    CMD_START_GAME_ROOM = 1,
    CMD_CHOOSE_CLASS = 2,
    CMD_PLAY_GAME_NORMAL = 3,
    CMD_PLAY_GAME_EXTEND = 4,
    CMD_TRY_CONFIRM_RESULT = 5,
    CMD_CREATE_ROOM = 6,
    CMD_COMBINE_PARTS = 7,
    CMD_PURCHASE_ITEM = 8,
    CMD_LOCATE_CURSOR = 9,
    CMD_CLEAR_POPUPS = 10
}

end -- Command_lua