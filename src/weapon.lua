----- 武器购买、使用相关常量设定 -----
local config = require("config")

local PRIMARY_WEAPON = 0
local SECONDARY_WEAPON = 1
local MELEE_WEAPON = 2
local NORMAL_EQUIPMENT = 3

local INSUFFICIENT_FUND_CONFIRM_BUTTON_X = 32750
local INSUFFICIENT_FUND_CONFIRM_BUTTON_Y = 34924

----- 全局变量 -----
g_AttackButton = config.LEFT_MOUSE_BUTTON

----- 局部变量 -----
local weapon_seq = MELEE_WEAPON -- 武器序列
----- 购买主武器 -----
local function BuyPrimaryWeapon()
    PressAndReleaseKey("b")
    Sleep(config.LONG_INTERVAL)
    PressAndReleaseKey("num8")
    Sleep(config.LONG_INTERVAL)
    PressAndReleaseKey("num6")
    Sleep(config.LONG_INTERVAL)
end

----- 购买副武器 -----
local function BuySecondaryWeapon()
    PressAndReleaseKey("b")
    Sleep(config.LONG_INTERVAL)
    PressAndReleaseKey("num1")
    Sleep(config.LONG_INTERVAL)
    PressAndReleaseKey("num3")
    Sleep(config.LONG_INTERVAL)
    PressAndReleaseKey("b")
    Sleep(config.LONG_INTERVAL)
    PressAndReleaseKey("7")
    Sleep(config.LONG_INTERVAL)
end

----- 购买近身武器 -----
local function BuyMeleeWeapon()
    PressAndReleaseKey("b")
    Sleep(config.LONG_INTERVAL) -- 买枪存在严重按键延迟，需要用长间隔
    PressAndReleaseKey("g")
end

----- 购买作战装备 -----
local function BuyNormalEquipment()
    PressAndReleaseKey("b")
    Sleep(config.LONG_INTERVAL)
    PressAndReleaseKey("8")
    Sleep(config.LONG_INTERVAL)
    PressAndReleaseKey("9")
    Sleep(config.LONG_INTERVAL)
end

----- 购买失败处理 -----
local function InCaseBuyWeaponFail()
    MoveMouseTo(INSUFFICIENT_FUND_CONFIRM_BUTTON_X, INSUFFICIENT_FUND_CONFIRM_BUTTON_Y) -- 购买失败的确认按钮
    Sleep(config.SHORT_INTERVAL)
    PressAndReleaseMouseButton(1)
    Sleep(config.SHORT_INTERVAL)
    PressAndReleaseKey("0")
    Sleep(config.SHORT_INTERVAL)
end

----- 切换武器 -----
local function switch_weapon(seq)
    if (seq == PRIMARY_WEAPON)
    then
        g_AttackButton = config.RIGHT_MOUSE_BUTTON
    elseif (seq == SECONDARY_WEAPON)
    then
        g_AttackButton = config.LEFT_MOUSE_BUTTON
    elseif (seq == MELEE_WEAPON) then
        g_AttackButton = config.LEFT_MOUSE_BUTTON
    elseif (seq == NORMAL_EQUIPMENT)
    then
        g_AttackButton = config.LEFT_MOUSE_BUTTON
    end
    
    for i = 0, 2
    do
        PressAndReleaseKey(toString(seq + 1))
        Sleep(config.SHORT_INTERVAL)
    end
end 