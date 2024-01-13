module = {}
local weapon = require("weapon")
local config = require("config")
local walk = require("walk")
local attack = require("attack")

----- 角色选择 -----
local CLASS_OPTION = "3"

----- 轮次时间设定 -----
local ROUND_TIME = 60 * 1000 -- 一轮时间为60秒（不是回合时间，而是程序的重置时间）

local round_timer = -ROUND_TIME

local function QuadrupleEscape()
    PressAndReleaseKey("escape")
    Sleep(config.SHORT_INTERVAL)
    PressAndReleaseKey("escape")
    Sleep(config.SHORT_INTERVAL)
    PressAndReleaseKey("escape")
    Sleep(config.SHORT_INTERVAL)
    PressAndReleaseKey("escape")
    Sleep(config.SHORT_INTERVAL)
    MoveMouseTo(config.ESC_MENU_CANCEL_X, config.ESC_MENU_CANCEL_Y) -- 鼠标点击ESC菜单取消按钮，防止ESC界面阻塞
    Sleep(config.LONG_LONG_INTERVAL)
    PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
    Sleep(config.SHORT_INTERVAL)
end

function ChooseClass()
    for i = 0, 3
    do
        PressAndReleaseKey(CLASS_OPTION) -- 选定角色
        Sleep(config.SHORT_INTERVAL)
    end
end

function StartGame()
    MoveMouseTo(config.GAMESTART_X, config.GAMESTART_Y)
    Sleep(config.REFRESH_INTERVAL)
    PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
    Sleep(config.LONG_LONG_INTERVAL)
end

function ConfirmSettlement()
    QuadrupleEscape() -- 关闭弹出的窗口
    MoveMouseTo(config.GAME_SETTLEMENT_CONFIRM_X, config.GAME_SETTLEMENT_CONFIRM_Y)
    Sleep(config.NORMAL_INTERVAL)
    PressAndReleaseMouseButton(CONFIRM_MOUSE_BUTTON)
    Sleep(config.NORMAL_INTERVAL)
end

function module.Rotate()
    while (true)
    do
        if (IsKeyLockOn("scrolllock"))
        then
            if (GetRunningTime() - round_timer > ROUND_TIME) -- 距离上一次执行该操作所耗费时间超过60s
            then
                ConfirmSettlement() -- 结算界面确认
                StartGame() -- 开始游戏
                round_timer = GetRunningTime() -- 更新定时器时间
                PressMouseButton(config.LEFT_MOUSE_BUTTON)
                Sleep(config.MINI_INTERVAL)
                ReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
                ChooseClass()
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
                round_timer = -ROUND_TIME
                break
            end
        end
    end
    OutputLogMessage("\nThe script is unloaded.\n")
    MoveMouseTo(config.SCREEN_MIDDLE_X, config.SCREEN_MIDDLE_Y)
end

return module