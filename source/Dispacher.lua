if (not Dispacher)
then
    local PATH = "C:/Users/Silve/Develop/CSOL-24H"
    assert(dofile(PATH .. "source/Rould.lua"), "source\\Round.lua not found.")
    assert(dofile(PATH .. "source/Player.lua"), "source\\Player.lua not found.")
    Dispacher = {}
    function Dispacher:dispatch()
        if (COMMAND == "ROUND_CONFIRM") -- 结算
        then
            Round:resultConfirm() -- 结算测试
        elseif (COMMAND == "ROUND_START") -- 开始游戏
        then
            Round:startGame() -- 点击“开始游戏”按钮
        elseif (COMMAND == "ROUND_PLAY")
        then
            Round:play(PLAYER)
        elseif (COMMAND == "ROUND_HALT") -- 暂停该回合
        then
            Runtime:sleep(1000)
        end
    end

end