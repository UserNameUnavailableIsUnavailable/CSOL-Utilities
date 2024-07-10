if (not Command_lua)
then
Command_lua = true
Command = {
    CMD_START_GAME_ROOM = 0x01,
    CMD_CHOOSE_CLASS = 0x02,
    CMD_PLAY_NORMAL = 0x80,
    CMD_PLAY_EXTEND = 0x200,
    CMD_TRY_CONFIRM_RESULT = 0x1000,
    CMD_CREATE_ROOM = 0x10000
}
end -- Command_lua