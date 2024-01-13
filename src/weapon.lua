module = {}


local config = require("config")
----- 武器购买、使用相关常量设定 -----
local PRIMARY_WEAPON = 0
local SECONDARY_WEAPON = 1
local MELEE_WEAPON = 2
local NORMAL_EQUIPMENT = 3

----- 手雷相关 -----
----- 使用天使雷辅助杀敌 -----
-- 机制：天使雷展开25秒，R，关闭10秒，如此循环
GRENADE_ON = 1
GRENADE_OFF = 0
GRENADE_ON_TIME = 25 * 1000
GRENADE_OFF_TIME = 10 * 1000

----- 攻击按键（鼠标左键或右键，根据武器类型改变） -----
module.g_AttackButton = config.LEFT_MOUSE_BUTTON

local grenade_on_moment = 0
local grenade_off_moment = 0
local grenade_switch = GRENADE_ON
-- 资金不足无法购买会弹出对话框，需要确认（能不ESC就不ESC）
local INSUFFICIENT_FUND_CONFIRM_BUTTON_X = 32750
local INSUFFICIENT_FUND_CONFIRM_BUTTON_Y = 34924

-- 各武器购买按键序列
local BUY_PRIMARY_WEAPON = {"b", "8", "6"}
local BUY_SECONDARY_WEAPON = {"b", "1", "3", "b", "7"} -- 视情况确定要不要购买子弹
local BUY_MELEE_WEAPON = {"b", "g"}
local BUY_NORMAL_EQUIPMENT = {"b", "8", "9"}


----- 局部变量 -----
local buy_weapon_seq = MELEE_WEAPON -- 购买武器类型（购买什么武器）
local attack_weapon_seq = MELEE_WEAPON -- 攻击武器类型（选用什么武器攻击）

----- 执行购买操作 -----
local function Buy(key_sequence)
    for i = 1, #key_sequence -- LUA下标从1开始
    do
        PressAndReleaseKey(key_sequence[i])
        Sleep(config.LONG_INTERVAL) -- 购买武器延迟较长
    end
end

----- 购买主武器 -----
local function BuyPrimaryWeapon()
    Buy(BUY_PRIMARY_WEAPON)
end

----- 购买副武器 -----
local function BuySecondaryWeapon()
    Buy(BUY_SECONDARY_WEAPON)
end

----- 购买近身武器 -----
local function BuyMeleeWeapon()
    Buy(BUY_MELEE_WEAPON)
end

----- 购买作战装备 -----
local function BuyNormalEquipment()
    Buy(BUY_NORMAL_EQUIPMENT)
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

local function ToggleGrenade() -- 天使雷开关
    local turn_off = grenade_switch == GRENADE_ON and GetRunningTime() - grenade_on_moment > GRENADE_ON_TIME
    local turn_on = grenade_switch == GRENADE_OFF and GetRunningTime() - grenade_off_moment > GRENADE_OFF_TIME

    if (turn_off or turn_on)
    then
        PressAndReleaseKey(NORMAL_EQUIPMENT)
        Sleep(config.MINI_INTERVAL)
    else
        return
    end

    if (turn_off)
    then
        PressAndReleaseKey("r")
        grenade_off_moment = GetRunningTime()
        Sleep(config.LONG_LONG_INTERVAL)
        grenade_switch = GRENADE_OFF
    elseif (turn_on)
    then
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        grenade_on_moment = GetRunningTime()
        Sleep(config.LONG_LONG_INTERVAL)
        grenade_switch = GRENADE_ON
    end

    PressAndReleaseKey(MELEE_WEAPON)
    Sleep(config.MINI_INTERVAL)
    g_AttackButton = config.LEFT_MOUSE_BUTTON
end

----- 切换武器 -----
local function SwitchWeapon()
    math.randomseed(GetRunningTime())
    local rand = math.random()
    ----- 依据概率选择使用哪个武器攻击 -----
    if (rand < 0.1)
    then
        attack_weapon_seq = PRIMARY_WEAPON -- 10%概率使用主武器（星轮右键）
    elseif (rand < 0.4)
    then
        attack_weapon_seq = SECONDARY_WEAPON -- 30%概率使用副武器（天驱左键）
    else
        attack_weapon_seq = MELEE_WEAPON -- 60%概率使用作战装备（天使雷）
    end

    if (attack_weapon_seq == PRIMARY_WEAPON)
    then
        g_AttackButton = config.RIGHT_MOUSE_BUTTON
    elseif (attack_weapon_seq == SECONDARY_WEAPON)
    then
        g_AttackButton = config.LEFT_MOUSE_BUTTON
    elseif (attack_weapon_seq == MELEE_WEAPON) then
        g_AttackButton = config.LEFT_MOUSE_BUTTON
    elseif (attack_weapon_seq == NORMAL_EQUIPMENT)
    then
        ToggleGrenade()
        return
    end
    for i = 0, 2
    do
        PressAndReleaseKey(tostring(attack_weapon_seq + 1))
        Sleep(config.SHORT_INTERVAL)
    end
end



----- 模块 -----

----- 购买武器 -----
function module.BuyWeapon()
    if (weapon_seq == PRIMARY_WEAPON)
    then
        BuyPrimaryWeapon()
        weapon_seq = SECONDARY_WEAPON
    elseif (weapon_seq == SECONDARY_WEAPON)
    then
        BuySecondaryWeapon()
        weapon_seq = MELEE_WEAPON
    elseif (weapon_seq == MELEE_WEAPON)
    then
        BuyMeleeWeapon()
        weapon_seq = NORMAL_EQUIPMENT
    elseif (weapon_seq == NORMAL_EQUIPMENT)
    then
        BuyNormalEquipment()
        weapon_seq = PRIMARY_WEAPON
    end
    InCaseBuyWeaponFail()
    SwitchWeapon()
end
return module