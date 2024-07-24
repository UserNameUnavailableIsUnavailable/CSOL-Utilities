if (not Interpreter_lua)
then
Interpreter_lua = true

local function expired()
    CmdTime = CmdTime or 0
    local current_time = DateTime:get_local_timestamp() -- 本地时间戳
    return math.abs(current_time - CmdTime) > 5
end

function Interpreter()
    local previous_command = Command.NOP
    while (true)
    do
        Load("$~cmd.lua")
        Cmd = Cmd or Command.CMD_NOP
        -- 文件中的命令时间戳与当前时间的差值达到 5 秒认为文件中的命令无效
        if (Cmd == Command.CMD_START_GAME_ROOM and not expired())
        then
            Executor:start_game_room()
        elseif (Cmd == Command.CMD_CHOOSE_CLASS and not expired())
        then
            Executor:choose_class()
        elseif (Cmd == Command.CMD_PLAY_GAME_NORMAL and not expired())
        then
            Executor:try_confirm()
            Player:buy_part_weapon(PartWeaponList)
            Player:play(DefaultWeaponList)
        elseif (Cmd == Command.CMD_PLAY_GAME_EXTEND and not expired())
        then
            Executor:try_confirm()
            Player:use_special_weapon(SpecialWeapon)
            Player:play(ExtendedWeaponList)
        elseif (Cmd == Command.CMD_CREATE_ROOM and not expired())
        then
            Executor:create_game_room()
        elseif (Cmd == Command.CMD_COMBINE_PARTS and not expired())
        then
            Executor:combine_parts()
        elseif (Cmd == Command.CMD_PURCHASE_ITEM and not expired())
        then
            local buy_button_x = 0
            local buy_button_y = 0
            if (previous_command ~= Cmd) -- 对于新发出的命令，需要更新鼠标光标位置
            then
                buy_button_x, buy_button_y = Mouse:locate_cursor()
            end
            Executor:purchase_item(buy_button_x, buy_button_y)
        elseif (Cmd == Command.CMD_LOCATE_CURSOR and not expired())
        then
            Executor:locate_cursor()
        end
        Runtime:sleep(10)
    end
end

end -- Interpreter_lua