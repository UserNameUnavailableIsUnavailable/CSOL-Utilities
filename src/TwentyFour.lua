local Weapon = {
    name = '', -- 武器名称
    purchaseKeySeq = {}, -- 武器购买按键序列
    attackMouseButton = {}, -- 攻击按键
    new = function (self, obj)
        obj = obj or {} -- 确保obj非nil
        self.__index = self
        setmetatable(obj, self)
        return obj
    end,
    purchase = function (self)
        for key in self.purchaseKeySeq
        do
            PressAndReleaseKey(key)
            Sleep(config.LONG_INTERVAL)
        end
    end,
    attack = function (self)

    end
}


util = {}

function util.ClickButton(x, y)
    MoveMouseTo(x, y)
    Sleep(config.SHORT_INTERVAL)
    PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
    Sleep(config.SHORT_INTERVAL)
end

function util.ExponentialGrowthPair(p, q)
    if (p > 0 and q > 0 and p + q <= 1)
    then
        if (2 * p + q <= 1 and p + 2 * q <= 1)
        then
            p = 2 * p
            q = 2 * q
        elseif (2 * p + q <= 1)
        then
            p = 2 * p
        elseif (p + 2 * q <= 1)
        then
            q = 2 * q
        end
    end
    return p, q
end

config = {}

config.GAMESTART_X = 47025
config.GAMESTART_Y = 42030
-- 游戏结算页面的“确认”按钮
config.GAME_SETTLEMENT_CONFIRM_X = 44976
config.GAME_SETTLEMENT_CONFIRM_Y = 51748
-- ESC菜单的“取消”按钮
config.ESC_MENU_CANCEL_X = 32375
config.ESC_MENU_CANCEL_Y = 43913

-- 屏幕中央
config.SCREEN_MIDDLE_X = 32768
config.SCREEN_MIDDLE_Y = 32768
----- 延时设定 -----
config.MINI_INTERVAL = 10
config.SHORT_INTERVAL = 50
config.NORMAL_INTERVAL = 100
config.MEDIUM_INTERVAL = 150
config.LONG_INTERVAL = 200
config.LONG_LONG_INTERVAL = 500
config.REFRESH_INTERVAL = 1000

----- 鼠标按键 -----
config.LEFT_MOUSE_BUTTON = 1
config.MIDDLE_MOUSE_BUTTON = 2
config.RIGHT_MOUSE_BUTTON = 3


weapon = {}

----- 武器购买、使用相关常量设定 -----
weapon.PRIMARY_WEAPON = 0
weapon.SECONDARY_WEAPON = 1
weapon.MELEE_WEAPON = 2
weapon.NORMAL_EQUIPMENT = 3
----- 各武器购买概率 -----
weapon.primary_possibility = 0.02
weapon.secondary_possibility = 0.02
weapon.melee_possibility = 1
weapon.MAX_PRIMARY_POSSIBILITY = 0.33
weapon.MAX_SECONDARY_POSSIBILITY = 0.33
weapon.MIN_MELEE_POSSIBILITY = 0.3
weapon.weapon_seq = weapon.MELEE_WEAPON
----- 手雷相关 -----
----- 使用天使雷辅助杀敌 -----
-- 机制：天使雷展开25秒，R，关闭10秒，如此循环
weapon.GRENADE_ON = 1
weapon.GRENADE_OFF = 0
weapon.GRENADE_ON_TIME = 24 * 1000
weapon.GRENADE_OFF_TIME = 9 * 1000
weapon.GRENADE_DELAY = 600
----- 攻击按键（鼠标左键或右键，根据武器类型改变） -----
weapon.attack_button = config.LEFT_MOUSE_BUTTON -- 攻击按键，多人挂机状态有效

weapon.grenade_on_moment = -114514
weapon.grenade_off_moment = -114514

weapon.grenade_switch = weapon.GRENADE_ON
-- 资金不足无法购买会弹出对话框，需要确认（能不ESC就不ESC）
weapon.INSUFFICIENT_FUND_CONFIRM_BUTTON_X = 32750
weapon.INSUFFICIENT_FUND_CONFIRM_BUTTON_Y = 34924

-- 各武器购买按键序列
weapon.MULTI_PLAYERS_BUY_PRIMARY = {
    {"b", "8", "3"}, -- 1
    {"b", "8", "6"}, -- 2
    {"b", "2", "1"}, -- 3
    {"b", "2", "2"} -- 4
}
weapon.MULTI_PLAYERS_PRIMARY_ATTACK = {
    config.LEFT_MOUSE_BUTTON, -- 1
    config.RIGHT_MOUSE_BUTTON, -- 2
    config.LEFT_MOUSE_BUTTON, -- 3
    config.LEFT_MOUSE_BUTTON -- 4
}
weapon.MULTI_PLAYERS_BUY_SECONDARY = {
    {"b", "1", "1"}, -- 1
    {"b", "1", "2"}, -- 2
    {"b", "1", "3"}, -- 3
    {"b", "1", "4"}, -- 4
    {"b", "1", "5"} -- 5
}
weapon.MULTI_PLAYERS_SECONDARY_ATTACK = {
    config.LEFT_MOUSE_BUTTON, -- 1
    config.LEFT_MOUSE_BUTTON, -- 2
    config.LEFT_MOUSE_BUTTON, -- 3
    config.LEFT_MOUSE_BUTTON, -- 4
    config.LEFT_MOUSE_BUTTON -- 5
}
weapon.SINGLE_PLAYER_BUY_PRIMARY = {"b", "j", "2", "4"}
weapon.SINGLE_PLAYER_BUY_SECONDARY = {"b", "j", "1", "2"}

weapon.BUY_MELEE_WEAPON = {"b", "g"}
weapon.BUY_NORMAL_EQUIPMENT = {"b", "8", "9"}

weapon.BUY_AC = {"b", "8", "2"}

weapon.buy_weapon_seq = weapon.MELEE_WEAPON -- 购买武器类型（购买什么配件武器），单人挂机状态有效
weapon.attack_weapon_seq = weapon.MELEE_WEAPON -- 攻击武器类型（选用什么武器攻击），多人挂机状态有效
weapon.melee_lock = false -- 锁定使用近战武器（单人挂机状态或回合加载状态时为true）

-- 死亡视角下预购买菜单“重复购买”按钮位置
weapon.PREPURCHASE_REBUY_X = 31692
weapon.PREPURCHASE_REBUY_Y = 43123

----- 重置武器选项，使用近战武器 -----
function weapon.Reset()
    weapon.ResetPossibility()
    weapon.attack_weapon_seq = weapon.MELEE_WEAPON
    weapon.attack_button = config.LEFT_MOUSE_BUTTON
    weapon.melee_lock = true
end

function weapon.ResetPossibility()
    if (not IsKeyLockOn("capslock"))
    then
        weapon.primary_possibility = 0.05
        weapon.secondary_possibility = 0.05
    else
        weapon.primary_possibility = 0
        weapon.secondary_possibility = 0
    end
end

----- 设置武器选项，允许使用主副武器 -----
function weapon.Set()
    weapon.melee_lock = false
end

----- 执行购买操作 -----
function weapon.Buy(key_sequence)
    for i = 1, #key_sequence -- LUA下标从1开始
    do
        PressAndReleaseKey(key_sequence[i])
        Sleep(config.NORMAL_INTERVAL) -- 购买武器延迟较长
    end
    weapon.InCaseBuyWeaponFail() -- 购买失败处理
end

weapon.primary_weapon_option = 0
----- 购买主武器 -----
function weapon.BuyPrimaryWeapon()
    if (weapon.primary_possibility > 0)
    then
        weapon.Buy(weapon.MULTI_PLAYERS_BUY_PRIMARY[weapon.primary_weapon_option + 1])
        weapon.attack_button = weapon.MULTI_PLAYERS_PRIMARY_ATTACK[weapon.primary_weapon_option + 1]
        weapon.primary_weapon_option = (weapon.primary_weapon_option + 1) % #weapon.MULTI_PLAYERS_BUY_PRIMARY
        weapon.SwitchTo(weapon.PRIMARY_WEAPON)
    else
        weapon.Buy(weapon.SINGLE_PLAYER_BUY_PRIMARY)
        weapon.SwitchTo(weapon.MELEE_WEAPON)
    end
    
end

weapon.secondary_weapon_option = 0
----- 购买副武器 -----
function weapon.BuySecondaryWeapon()
    if (weapon.secondary_possibility > 0)
    then
        weapon.Buy(weapon.MULTI_PLAYERS_BUY_SECONDARY[weapon.secondary_weapon_option + 1])
        weapon.attack_button = weapon.MULTI_PLAYERS_SECONDARY_ATTACK[weapon.secondary_weapon_option + 1]
        weapon.secondary_weapon_option = (weapon.secondary_weapon_option + 1) % #weapon.MULTI_PLAYERS_BUY_SECONDARY
        weapon.SwitchTo(weapon.SECONDARY_WEAPON)
    else
        weapon.Buy(weapon.SINGLE_PLAYER_BUY_SECONDARY)
        weapon.SwitchTo(weapon.MELEE_WEAPON)
    end
end

----- 购买近身武器 -----
function weapon.BuyMeleeWeapon()
    weapon.Buy(weapon.BUY_MELEE_WEAPON)
    weapon.attack_button = config.LEFT_MOUSE_BUTTON
    weapon.SwitchTo(weapon.MELEE_WEAPON)
end

----- 购买作战装备 -----
function weapon.BuyNormalEquipment()
    weapon.Buy(weapon.BUY_NORMAL_EQUIPMENT)    
end

----- 购买失败处理 -----
function weapon.InCaseBuyWeaponFail()
    util.ClickButton(weapon.INSUFFICIENT_FUND_CONFIRM_BUTTON_X, weapon.INSUFFICIENT_FUND_CONFIRM_BUTTON_Y) -- 购买失败的确认按钮
    util.ClickButton(weapon.PREPURCHASE_REBUY_X, weapon.PREPURCHASE_REBUY_Y) -- 死亡状态购买打开预购菜单，需要取消购买
    PressAndReleaseKey("0") -- 关闭正常购买菜单按“0”
    Sleep(config.SHORT_INTERVAL)
end

function weapon.ToggleGrenade() -- 天使雷开关
    local turn_off = weapon.grenade_switch == weapon.GRENADE_ON and GetRunningTime() - weapon.grenade_on_moment > weapon.GRENADE_ON_TIME
    local turn_on = weapon.grenade_switch == weapon.GRENADE_OFF and GetRunningTime() - weapon.grenade_off_moment > weapon.GRENADE_OFF_TIME

    if (turn_off or turn_on) 
    then
        PressAndReleaseKey(tostring(weapon.NORMAL_EQUIPMENT + 1)) -- 切换到作战装备
        Sleep(weapon.GRENADE_DELAY)
    else -- 部队天使雷进行操作
        weapon.BuyNormalEquipment() -- 预先购买，等到需要时可以直接用
    end
    if (turn_off)
    then
        MoveMouseRelative(0, 5000)
        Sleep(config.NORMAL_INTERVAL)
        PressAndReleaseKey("r")
        Sleep(config.LONG_LONG_INTERVAL)
        MoveMouseRelative(0, -5000)
        weapon.grenade_off_moment = GetRunningTime()
        Sleep(weapon.GRENADE_DELAY)
        weapon.grenade_switch = weapon.GRENADE_OFF
    elseif (turn_on)
    then

        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        weapon.grenade_on_moment = GetRunningTime()
        Sleep(config.GRENADE_DELAY)
        weapon.grenade_switch = weapon.GRENADE_ON
    end
    PressAndReleaseKey(tostring(weapon.MELEE_WEAPON + 1))
    Sleep(config.MINI_INTERVAL)
    weapon.attack_button = config.LEFT_MOUSE_BUTTON
end

----- 购买护甲 -----
function weapon.BuyAC()
    weapon.Buy(weapon.BUY_AC)
end

-- weapon.next_melee = true -- 一旦上一次使用了主武器，则下一次必定使用近战，防止意外死亡

----- 切换武器 -----
function weapon.SwitchWeapon()
    math.randomseed(GetRunningTime())
    local rand = math.random()
    ----- 依据概率选择使用哪个武器攻击 -----
    if (rand < weapon.primary_possibility) -- and not weapon.next_melee)
    then
        weapon.attack_weapon_seq = weapon.PRIMARY_WEAPON -- 概率使用主武器
        weapon.BuyPrimaryWeapon()
        --weapon.next_melee = true
    elseif (rand < weapon.primary_possibility + weapon.secondary_possibility) --  and not weapon.next_melee
    then
        weapon.attack_weapon_seq = weapon.SECONDARY_WEAPON -- 概率使用副武器
        weapon.BuySecondaryWeapon()
        --weapon.next_melee = false
    else
        weapon.attack_weapon_seq = weapon.MELEE_WEAPON -- 概率使用近战
        weapon.BuyMeleeWeapon()
        -- weapon.next_melee = false
    end
    Sleep(config.NORMAL_INTERVAL)
    if (weapon.primary_possibility + weapon.secondary_possibility == 0)
    then
        weapon.buy_weapon_seq = (weapon.buy_weapon_seq + 1) % 4
        if (weapon.buy_weapon_seq == weapon.PRIMARY_WEAPON)
        then
            weapon.BuyPrimaryWeapon()
        elseif (weapon.buy_weapon_seq == weapon.SECONDARY_WEAPON)
        then
            weapon.BuySecondaryWeapon()
        elseif (weapon.buy_weapon_seq == weapon.MELEE_WEAPON)
        then
            weapon.BuyMeleeWeapon()
        end
        weapon.SwitchTo(weapon.MELEE_WEAPON)
    end
end

function weapon.SwitchTo(number)
    if (number >= 0 and number <= 3)
    then
        PressAndReleaseKey(tostring(number + 1)) -- 切换到选定的武器
        Sleep(config.NORMAL_INTERVAL)
    end
end

function weapon.UpdatePossibility()
    weapon.primary_possibility = weapon.primary_possibility * 3
    weapon.secondary_possibility = weapon.secondary_possibility * 3
    if (weapon.primary_possibility > weapon.MAX_PRIMARY_POSSIBILITY)
    then
        weapon.primary_possibility = weapon.MAX_PRIMARY_POSSIBILITY
    end
    if (weapon.secondary_possibility > weapon.MAX_SECONDARY_POSSIBILITY)
    then
        weapon.secondary_possibility = weapon.MAX_SECONDARY_POSSIBILITY
    end
end

attack = {}
----- 攻击相关 -----
function attack.StartAttack()
    PressMouseButton(weapon.attack_button) -- 开始攻击
    Sleep(config.SHORT_INTERVAL)
end

function attack.StopAttack()
    ReleaseMouseButton(weapon.attack_button) -- 结束攻击
    Sleep(config.SHORT_INTERVAL)
    if (not weapon.melee_lock)
    then
        weapon.ToggleGrenade() -- 一轮攻击结束后处理天使雷相关操作
    end
end


move = {}
----- 行走相关设定 -----
move.WALK = 0
move.STRAFE = 1
move.walk_direction = 1 -- 0表示向前，1表示向后
move.strafe_direction = 1 -- 0表示向右，1表示向左
move.yaw_direction = 1 -- 0表示视角右转，1表示左转
move.pitch_direction = 1 -- 0表示视角向下，1表示视角向上

move.YAW_SPEED = 100
move.PITCH_SPEED = 0

move.randomseed = {100, 200, 300, 400, 500, 600}
move.direction = {1, 1, 1, 1, 1}

function move.ChangeDirection()
    move.randomseed[5] = GetRunningTime()
    for i = 1, #move.randomseed - 1
    do
        move.randomseed[i] = move.randomseed[i + 1]
    end
    for i = 1, #move.direction
    do
        math.randomseed(move.randomseed[i])
        move.direction[i] = math.random(0, 1)
    end
    move.walk_direction = move.direction[1]
    move.strafe_direction = move.direction[2]
    move.yaw_direction = move.direction[3]
    move.pitch_direction = move.direction[4]
end

function move.StartMove()
    if (move.walk_direction == 0)
    then
        PressKey("w")
    else
        PressKey("s")
    end
    if (move.strafe_direction == 0)
    then
        PressKey("d")
    else
        PressKey("a")
    end
end

function move.StopMove()
    if (move.walk_direction == 0)
    then
        ReleaseKey("w")
    else
        ReleaseKey("s")
    end
    if (move.strafe_direction == 0)
    then
        ReleaseKey("d")
    else
        ReleaseKey("a")
    end
    move.ChangeDirection()
end

function move.ChangeView()
    local yaw_coeff
    local pitch_coeff
    math.randomseed(GetRunningTime())
    yaw_direction = math.random(0, 1)
    Sleep(config.SHORT_INTERVAL)
    -- 控制转向
    
    if (yaw_direction == 0)
    then
        yaw_coeff = (1.0 - 0.8 * math.random())
    else
        yaw_coeff = - (1.0 - 0.8 * math.random())
    end

    if (move.pitch_direction == 0)
    then
        pitch_coeff = 1
    else
        pitch_coeff = -1
    end

    for i = 1, 400 do
        MoveMouseRelative(math.ceil(yaw_coeff * move.YAW_SPEED), pitch_coeff * move.PITCH_SPEED) -- 随机向左或向右转
        Sleep(config.MINI_INTERVAL)
    end
    PressAndReleaseKey("r") -- 回合重置
end


round = {}
----- 角色选择 -----
round.CLASS_OPTION = "3"

----- 轮次时间设定 -----
round.ROUND_TIME = 40 * 1000 -- 一轮时间为60秒（不是真正的回合时间，而是代表程序状态重置时间）
round.round_timer = -round.ROUND_TIME
round.LOAD_TIME = 20 * 1000 -- 加载时间为20秒
function round.Escape(N)
    for i = 1, N
    do
        PressAndReleaseKey("escape")
        Sleep(config.SHORT_INTERVAL)
    end
    util.ClickButton(config.ESC_MENU_CANCEL_X, config.ESC_MENU_CANCEL_Y) -- 鼠标点击ESC菜单取消按钮，防止ESC界面阻塞
end

function round.ChooseClass()
    for i = 0, 9
    do
        PressAndReleaseKey(round.CLASS_OPTION) -- 选定角色
        Sleep(config.SHORT_INTERVAL)
    end
end

function round.StartGame()
    util.ClickButton(config.GAMESTART_X, config.GAMESTART_Y)
end

function round.ConfirmSettlement()
    util.ClickButton(config.SCREEN_MIDDLE_X, config.SCREEN_MIDDLE_Y)
    round.Escape(4) -- 关闭弹出的窗口
    util.ClickButton(config.GAME_SETTLEMENT_CONFIRM_X, config.GAME_SETTLEMENT_CONFIRM_Y)
    Sleep(config.LONG_LONG_INTERVAL)
end

----- 重置状态，等待游戏加载，如正在游戏中，则此期间只使用近战武器 -----
function round.Reset() -- 期间不能进行任何带数字键的购买，否则会选错人物
    local timer = GetRunningTime() -- 记录重置开始时间
    weapon.Reset()
    local elapsed
    repeat
        elapsed = GetRunningTime() - timer
        -- 购买近战（快捷购买，不带数字键），这里不使用BuyMeleeWeapon()函数，BuyMeleeWeapon()函数会多按一个“0”
        -- 进而导致角色选错
        for i = 1, #weapon.BUY_MELEE_WEAPON 
        do
            PressAndReleaseKey(weapon.BUY_MELEE_WEAPON[i])
            Sleep(config.LONG_INTERVAL)
        end
        -- 存活状态下购买近战绝对不会失败，但是可能会在死亡状态下发起购买而弹出预购菜单，此时需要取消
        util.ClickButton(weapon.PREPURCHASE_REBUY_X, weapon.PREPURCHASE_REBUY_Y)
        PressAndReleaseKey("f2")
        Sleep(config.SHORT_INTERVAL) -- 按F2可购买小甲（不能用购买菜单或装备快捷购买菜单，否则错选人物）
        weapon.SwitchTo(weapon.MELEE_WEAPON)
        move.StartMove()
        attack.StartAttack()
        move.ChangeView()
        attack.StopAttack()
        move.StopMove()
        OutputLogMessage("Waiting for reset, time elapsed = %d\n", elapsed)
        if (not IsKeyLockOn("scrolllock"))
        then
            OutputLogMessage("Reset interrupted.\n")
            return false
        end
    until (elapsed > round.LOAD_TIME)
    weapon.Set()
    return true
end

round.INIT_SMALL_MEDICAL_KIT_POSSIBILITY = 0.04
round.INIT_LARGE_MEDICAL_KIT_POSSIBILITY = 0.03

round.small_medical_kit_possibility = 0.04
round.large_medical_kit_possibility = 0.03



function round.UseMedicalKit()
    math.randomseed(GetRunningTime())
    local rand = math.random()
    if (rand <= round.small_medical_kit_possibility)
    then
        round.small_medical_kit_possibility = round.INIT_SMALL_MEDICAL_KIT_POSSIBILITY
        round.large_medical_kit_possibility = round.INIT_LARGE_MEDICAL_KIT_POSSIBILITY
        PressAndReleaseKey("5")
        Sleep(config.SHORT_INTERVAL)
    elseif(rand <= round.small_medical_kit_possibility + round.large_medical_kit_possibility)
    then
        round.small_medical_kit_possibility = round.INIT_SMALL_MEDICAL_KIT_POSSIBILITY
        round.large_medical_kit_possibility = round.INIT_LARGE_MEDICAL_KIT_POSSIBILITY
        PressAndReleaseKey("6")
        Sleep(config.SHORT_INTERVAL)
    else
        round.small_medical_kit_possibility, round.large_medical_kit_possibility = 
        util.ExponentialGrowthPair(round.small_medical_kit_possibility, round.large_medical_kit_possibility)
    end
end

function round.Rotate()
    local suspended = false
    OutputLogMessage("Enter into Rotate().\n")
    local begin_new_round = true
    while (true)
    do    
        ::ROTATE_WHILE_LOOP_START::
        if (IsKeyLockOn("scrolllock"))
        then

            if (suspended) -- 由挂起态转入运行态
            then
                OutputLogMessage("Resume Rotate().\n")
                suspended = false
            end

            if (begin_new_round)
            then
                round.StartGame() -- 点击“开始游戏”按钮
                if (not round.Reset()) -- 重置回合状态，等待进度条加载完成
                then
                    goto ROTATE_WHILE_LOOP_START
                end
                round.ChooseClass()
                round.round_timer = GetRunningTime() -- 更新定时器时间
                begin_new_round = false
            end

            weapon.SwitchWeapon() -- 切换武器
            move.StartMove() -- 开始行走
            attack.StartAttack() -- 开始攻击
            move.ChangeView() -- 视角旋转一段时间
            move.StopMove() -- 停止行走
            attack.StopAttack() -- 停止攻击
            weapon.BuyAC() -- 购买护甲
            math.randomseed(GetRunningTime())
            round.UseMedicalKit()
            weapon.UpdatePossibility() -- 更新概率

            if (GetRunningTime() - round.round_timer > round.ROUND_TIME) -- 距离上一次重置所耗费时间超过ROUND_TIME
            then
                round.ConfirmSettlement() -- 结算界面确认
                begin_new_round = true
            end

        else
            -- 右ctrl和右alt同时按下一段时间才停止
            if (not suspended) -- 从运行态转入挂起态
            then
                suspended = true
                OutputLogMessage("Suspend Rotate().\n")
            end
            Sleep(config.REFRESH_INTERVAL)
            if (IsModifierPressed("rctrl") and IsModifierPressed("ralt")) 
            then
                round.round_timer = -round.ROUND_TIME
                break
            end
        end
    end
    OutputLogMessage("Exit from Rotate().\n")
    MoveMouseTo(config.SCREEN_MIDDLE_X, config.SCREEN_MIDDLE_Y)
end


store = {}

----- 商店相关 -----

local STORE_PURCHASE_CHOICE_X = 22949
local STORE_PURCHASE_CHOICE_Y = 39479

local STORE_PURCHASE_X = 35858
local STORE_PURCHASE_Y = 42212

local STORE_PURCHASE_CONFIRM_X = 36712
local STORE_PURCHASE_CONFIRM_Y = 39357

function store.PurchaseItem(x, y)
    while (IsKeyLockOn("scrolllock"))
    do
        MoveMouseTo(x, y) -- 兑换按钮位置
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)

        MoveMouseTo(STORE_PURCHASE_CHOICE_X, STORE_PURCHASE_CHOICE_Y) -- 选项
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)

        MoveMouseTo(STORE_PURCHASE_X, STORE_PURCHASE_Y) -- 购买按钮
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)

        MoveMouseTo(STORE_PURCHASE_CONFIRM_X, STORE_PURCHASE_CONFIRM_Y) -- 确认
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.LONG_LONG_INTERVAL)

        PressAndReleaseKey("escape")

    end
end


craft = {}
----- 制造所相关 -----
-- 制造所的“自动添加按钮”
local CRAFT_PARTS_AUTO_FILL_X = 20012
local CRAFT_PARTS_AUTO_FILL_Y = 50533
-- 制造所的“开始合成”按钮
local CRAFT_PARTS_COMBINE_X = 25989
local CRAFT_PARTS_COMBINE_Y = 50654
--制造所配件合成的“重置”按钮
local CRAFT_PARTS_RESET_X = 33263
local CRAFT_PARTS_RESET_Y = 50594


----- 合成配件 -----
function craft.CombineParts()
    local counter = 0
    while (IsKeyLockOn("scrolllock"))
    do

        PressKey("enter")
        MoveMouseTo(CRAFT_PARTS_AUTO_FILL_X, CRAFT_PARTS_AUTO_FILL_Y)
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)
        MoveMouseTo(CRAFT_PARTS_COMBINE_X, CRAFT_PARTS_COMBINE_Y)
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)
        counter = counter + 1

        if (counter > 30)
        then
            MoveMouseTo(CRAFT_PARTS_RESET_X, CRAFT_PARTS_RESET_Y)
            PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
            counter = 0
            Sleep(config.MINI_INTERVAL)
        end

        ReleaseKey("enter")

    end

end


function OnEvent(event, arg, family)
    local x, y
    if (IsKeyLockOn("scrolllock"))
    then
        if (event == "MOUSE_BUTTON_PRESSED" and arg == 7 and family == "mouse")
        then
            round.Rotate()
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 6 and family == "mouse")
        then
            x, y = GetMousePosition()
            OutputLogMessage("Mouse Cursor Position = (%d, %d)\n", x, y)
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 5 and family == "mouse")
        then
            craft.CombineParts()
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 4 and family == "mouse")
        then
            x, y = GetMousePosition()
            store.PurchaseItem(x, y)
        end
    end
end