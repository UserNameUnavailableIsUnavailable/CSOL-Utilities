function Interpreter()
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
        elseif (Cmd == Command.CMD_COMBINE_PARTS)
        then
            Executor:combine_parts()
        end
        ::END_LOOP::
        Runtime:sleep(10) -- 每次执行完毕等待 10 毫秒
    end
end
