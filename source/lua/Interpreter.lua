function Interpreter()
    local lastTime = 0
    while (true)
    do
        Load("command.lua")
        -- do nothing if command not updated
        if (CmdTime == lastTime or CmdTime == 0)
        then
            goto END_LOOP
        end
        if (Cmd == CMD_START_GAME_ROOM)
        then
            StartGameRoom()
        end
        -- update time
        lastTime = CmdTime
        ::END_LOOP::
    end
end
