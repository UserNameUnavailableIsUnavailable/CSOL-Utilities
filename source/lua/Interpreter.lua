function Interpreter()
    local previous_command = Command.PAUSE -- 上一条命令的时间戳
    while (true)
    do
        local current_time = DateTime:get_local_timestamp() -- 本地时间戳
        Load("$~cmd.lua")
        -- 文件中的命令时间戳与当前时间的差值达到 5 秒认为文件中的命令无效
        if (math.abs(current_time - CmdTime) > 5)
        then
            goto END_LOOP
        end
        if (Cmd == Command.CMD_START_GAME_ROOM)
        then
            Executor:start_game_room()
        elseif (Cmd == Command.CMD_CHOOSE_CLASS)
        then
            Console:println(CmdTime .. " Choose class")
        elseif (Cmd == Command.CMD_TRY_CONFIRM_RESULT)
        then
            Console:println(CmdTime .. " Confirm")
        elseif (Cmd == Command.CMD_PLAY_GAME)
        then
            Console:println(CmdTime .. " Play")
        elseif (Cmd == Command.CMD_COMBINE_PARTS)
        then
            Executor:combine_parts()
        elseif (Cmd == Command.CMD_PURCHASE_ITEM)
        then
            local buy_button_x = 0
            local buy_button_y = 0
            if (previous_command ~= Cmd) -- 对于新发出的命令，需要更新鼠标光标位置
            then
                buy_button_x, buy_button_y = Mouse:locate_cursor()
            end
            Executor:purchase_item(buy_button_x, buy_button_y)
        end
        ::END_LOOP::
        Runtime:sleep(10) -- 每次执行完毕等待 10 毫秒
    end
end
