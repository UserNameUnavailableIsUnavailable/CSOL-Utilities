function Interpreter()
    local lastTime = 0
    while (true)
    do
        Load("command.lua")
        -- do nothing if command not updated
        if (gCmdTime == lastTime or gCmdTime == 0)
        then
            goto END_LOOP
        end
        if (gCmd == CMD_START_GAME_ROOM)
        then
            StartGameRoom()
        end
        -- update time
        lastTime = gCmdTime
        ::END_LOOP::
    end
end
