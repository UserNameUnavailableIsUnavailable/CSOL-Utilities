module = {}

local weapon = require("weapon")
local config = require("config")
local walk = require("walk")
local attack = require("attack")
local 
local round_timer = -round.ROUND_TIME

function module.rotate()
    while (true)
    do
        if (IsKeyLockOn("scrolllock"))
        then
            if (GetRunningTime() - round_timer > ROUND_TIME) -- 距离上一次执行该操作所耗费时间超过60s
            then
                round.ConfirmSettlement() -- 结算界面确认
                round.StartGame() -- 开始游戏
                round_timer = GetRunningTime() -- 更新定时器时间
                PressMouseButton(config.LEFT_MOUSE_BUTTON)
                Sleep(config.MINI_INTERVAL)
                ReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
                choose_class()
            end
            weapon.BuyWeapon() -- 执行购买武器操作
            walk.StartWalk() -- 开始行走
            attack.StartAttack() -- 开始攻击
            walk.Turn() -- 视角旋转一段时间
            walk.StopWalk() -- 停止行走
            attack.StopAttack() -- 停止攻击
        else
            -- 右ctrl和右alt同时按下一段时间才停止
            Sleep(config.REFRESH_INTERVAL)
            if (IsModifierPressed("rctrl") and IsModifierPressed("ralt")) then
                round_timer = -60000
                break
            end
        end
    end
    OutputLogMessage("\nThe script is unloaded.\n")
    MoveMouseTo(SCREEN_MIDDLE_X, SCREEN_MIDDLE_Y)
end

return module