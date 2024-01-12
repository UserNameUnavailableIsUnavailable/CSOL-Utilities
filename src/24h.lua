-- 游戏内按钮位置
GAMESTART_X = 47025
GAMESTART_Y = 42030
CONFIRM_X = 44976
CONFIRM_Y = 51748
ESC_MENU_CANCEL_X = 32375
ESC_MENU_CANCEL_Y = 43913
-- 游戏内延迟设定
MINI_INTERVAL = 10
SHORT_INTERVAL = 50
NORMAL_INTERVAL = 100
LONG_INTERVAL = 150
LONG_LONG_INTERVAL = 200
REFRESH_INTERVAL = 1000
-- 鼠标按键
LEFT_MOUSE_BUTTON = "1"
MIDDLE_MOUSE_BUTTON = "2"
RIGHT_MOUSE_BUTTON = "3"
CONFIRM_MOUSE_BUTTON = LEFT_MOUSE_BUTTON
attack_mouse_button = LEFT_MOUSE_BUTTON -- 攻击按键
-- 游戏内选项
CLASS_OPTION = "3" -- 选定三号角色
walk_direction = 1 -- 1表示W，0表示S
strafe_direction = 1 -- 1表示D，0表示A
turn_direction = 1 -- 1表示右转， 0表示左转
-- 配件武器选项
weapon_seq = 2
startgame_timer = -1200000
GRENADE_INTERVAL = 25 * 1000
AUTO_FILL_X = 20012
AUTO_FILL_Y = 50533

COMBINE_X = 25989
COMBINE_Y = 50654

RESET_X = 33263
RESET_Y = 50594

counter = 0;

function OnEvent(event, arg, family)
    if (IsKeyLockOn("scrolllock")) then
        if (event == "MOUSE_BUTTON_PRESSED" and arg == 7 and family == "mouse") then
            OutputLogMessage("\nThe script is loaded.\n")
            rotate()
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 6 and family == "mouse") then
            x, y = GetMousePosition()
            OutputLogMessage("(%d, %d)\n", x, y)
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 5 and family == "mouse") then
            combine()
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 4 and family == "mouse") then
            purchase()
        end
    end
end

function rotate()
    while (true) do
        if (IsKeyLockOn("scrolllock")) then
            if (GetRunningTime() - startgame_timer > 60000) -- 距离上一次执行该操作所耗费时间超过(60 * 3)s
            then
                confirm_settlement() -- 结算界面确认
                start_game() -- 开始游戏
                startgame_timer = GetRunningTime() -- 更新定时器时间
                PressMouseButton(1)
                Sleep(10)
                ReleaseMouseButton(1)
                choose_class()
            end
            buy_weapon() -- 执行购买武器操作
            start_walk() -- 开始行走
            start_attack() -- 开始攻击
            turn() -- 视角旋转一段时间
            stop_walk() -- 停止行走
            stop_attack() -- 停止攻击
        else
            -- 右ctrl和右alt同时按下一段时间才停止
            Sleep(REFRESH_INTERVAL)
            if (IsModifierPressed("rctrl") and IsModifierPressed("ralt")) then
                startgame_timer = -60000 * 3
                break
            end
        end
    end
    OutputLogMessage("\nThe script is unloaded.\n")
    MoveMouseTo(32768, 32768)
    Sleep(5000)
end

function start_game()
    MoveMouseTo(GAMESTART_X, GAMESTART_Y)
    Sleep(REFRESH_INTERVAL)
    PressAndReleaseMouseButton(CONFIRM_MOUSE_BUTTON)
    Sleep(LONG_LONG_INTERVAL)
end

function confirm_settlement() -- 结算界面确认
    quadruple_escape() -- 关闭弹出的窗口
    MoveMouseTo(CONFIRM_X, CONFIRM_Y)
    Sleep(NORMAL_INTERVAL)
    PressAndReleaseMouseButton(CONFIRM_MOUSE_BUTTON)
    Sleep(NORMAL_INTERVAL)
end

function choose_class()
    for i = 0, 3 do
        PressAndReleaseKey(CLASS_OPTION) -- 选定角色
        Sleep(SHORT_INTERVAL)
    end
end

function buy_weapon()
    math.randomseed(GetRunningTime())
    choose_class() -- 买枪之前先选择角色（再次减小选择角色与购买武器冲突的可能性，防止选错角色）
    PressAndReleaseKey("b")
    Sleep(LONG_INTERVAL) -- 买枪存在严重按键延迟，需要用长间隔
    PressAndReleaseKey("g")
    Sleep(LONG_INTERVAL)
    if (math.random() < 0.6) then
        if (weapon_seq == 0) -- 主武器
        then
            PressAndReleaseKey("b")
            Sleep(LONG_INTERVAL)
            -- PressAndReleaseKey("j")
            -- Sleep(LONG_INTERVAL)
            PressAndReleaseKey("num8")
            Sleep(LONG_INTERVAL)
            PressAndReleaseKey("num6")
            Sleep(LONG_INTERVAL)
            weapon_seq = (weapon_seq + 1) % 4
        elseif (weapon_seq == 1) -- 副武器
        then
            PressAndReleaseKey("b")
            Sleep(LONG_INTERVAL)
            -- PressAndReleaseKey("j")
            -- Sleep(LONG_INTERVAL)
            PressAndReleaseKey("num1")
            Sleep(LONG_INTERVAL)
            PressAndReleaseKey("num3")
            Sleep(LONG_INTERVAL)
            PressAndReleaseKey("b")
            Sleep(LONG_INTERVAL)
            PressAndReleaseKey("7")
            Sleep(LONG_INTERVAL)
            weapon_seq = (weapon_seq + 1) % 4
        elseif (weapon_seq == 2) -- 近战
        then
            weapon_seq = (weapon_seq + 1) % 4
        elseif (weapon_seq == 3) -- 手雷
        then
            PressAndReleaseKey("b")
            Sleep(LONG_INTERVAL)
            PressAndReleaseKey("8")
            Sleep(LONG_INTERVAL)
            PressAndReleaseKey("9")
            Sleep(LONG_INTERVAL)
            -- PressAndReleaseKey("b")
            -- Sleep(LONG_INTERVAL)
            -- PressAndReleaseKey("j")
            -- Sleep(LONG_INTERVAL)
            -- PressAndReleaseKey("num8")
            -- Sleep(LONG_INTERVAL)
            -- PressAndReleaseKey("num7")
            -- Sleep(LONG_INTERVAL)
            weapon_seq = (weapon_seq + 1) % 4
        end
    end
    MoveMouseTo(32750, 34924) -- 购买失败的确认按钮
    Sleep(50)
    PressAndReleaseMouseButton(1)
    Sleep(50)
    PressAndReleaseKey("0")
    Sleep(50)
    switch_weapon()
end

function quadruple_escape() -- 按4次ESC键，关闭可能弹出的窗口
    choose_class() -- 防止意外关闭角色选择窗口
    PressAndReleaseKey("escape")
    Sleep(SHORT_INTERVAL)
    PressAndReleaseKey("escape")
    Sleep(SHORT_INTERVAL)
    PressAndReleaseKey("escape")
    Sleep(SHORT_INTERVAL)
    PressAndReleaseKey("escape")
    Sleep(SHORT_INTERVAL)
    MoveMouseTo(ESC_MENU_CANCEL_X, ESC_MENU_CANCEL_Y) -- 鼠标点击ESC菜单取消按钮，防止ESC界面阻塞
    Sleep(LONG_LONG_INTERVAL)
    PressAndReleaseMouseButton(LEFT_MOUSE_BUTTON)
    Sleep(SHORT_INTERVAL)
end

function switch_weapon()
    math.randomseed(GetRunningTime())
    attack_weapon_rand = math.random()
    if (attack_weapon_rand <= 0.2) -- 20%的概率使用主武器
    then
        PressAndReleaseKey("1")
        attack_mouse_button = 3 -- 使用右键攻击
        Sleep(SHORT_INTERVAL)
    elseif (attack_weapon_rand <= 0.4) -- 20%的概率使用副武器
    then
        PressAndReleaseKey("2")
        attack_mouse_button = 1 -- 使用左键攻击
        Sleep(SHORT_INTERVAL)
    else -- 60%的概率使用近身武器
        PressAndReleaseKey("3") -- 切换刀
        attack_mouse_button = 1 -- 使用左键攻击
        Sleep(SHORT_INTERVAL)
    end
    toggle_grenade()
end

function start_attack()
    PressMouseButton(attack_mouse_button) -- 开始攻击
    Sleep(SHORT_INTERVAL)
end

function stop_attack()
    ReleaseMouseButton(attack_mouse_button) -- 结束攻击
    Sleep(SHORT_INTERVAL)
end

GRENADE_ON = 1
GRENADE_OFF = 0
GRENADE_ON_ELAPSED = 20 * 1000
GRENADE_OFF_ELAPSED = 10 * 1000
grenade_switch = GRENADE_ON
grenade_on_moment = 0
grenade_off_moment = 0
function toggle_grenade()
    PressAndReleaseKey("4") -- 切换到手雷
    Sleep(500)
    if (grenade_switch == GRENADE_ON) -- 天使雷处于开启状态
    then
        if (GRENADE_ON_ELAPSED < GetRunningTime() - grenade_on_moment) -- 到达规定时间，关闭天使的圈
        then
            PressAndReleaseKey("r") -- 放大，然后天使雷关闭
            grenade_off_moment = GetRunningTime() -- 记录天使雷关闭的时刻
            Sleep(500)
            grenade_switch = GRENADE_OFF -- 关闭状态
        end
    else
        if (GRENADE_OFF_ELAPSED < GetRunningTime() - grenade_off_moment) -- 充能时间足够，开启天使雷的圈
        then
            PressAndReleaseMouseButton(1) -- 开启天使的圈
            grenade_on_moment = GetRunningTime() -- 记录天使雷开启的时刻
            Sleep(500)
            grenade_switch = GRENADE_ON
        end
    end
end

function reset_grenade()
	grenade_switch = GRENADE_ON
	grenade_on_moment = 0
end

round_timer = -ROUND_TIME
ROUND_TIME = 60 * 1000 -- 一分钟视为一轮，重置一次
function round_reset()
	if (GetRunningTime() - round_timer> ROUND_TIME)
	then
		confirm_settlement() -- 结算界面确认
		start_game() -- 开始游戏
		round_timer = GetRunningTime() -- 更新定时器时间
		PressMouseButton(1)
		Sleep(10)
		ReleaseMouseButton(1)
		choose_class()
		reset_grenade()
	end
end

function turn()
    math.randomseed(GetRunningTime())
    if (math.random(0, 1) == 0) then
        turn_direction = turn_direction * (-1)
        PressAndReleaseKey("f2")
    end
    PressAndReleaseKey("r")
    Sleep(SHORT_INTERVAL)
    for i = 1, 400 do
        MoveMouseRelative(turn_direction * 65, 0) -- 随机向左或向右转
        Sleep(MINI_INTERVAL)
    end
end

function start_walk()
    if (walk_direction > 0) -- 前进/后退交替
    then
        PressKey("w")
    else
        PressKey("s")
    end
    if (strafe_direction > 0) then
        PressKey("d")
    else
        PressKey("a")
    end
end

function stop_walk()
    if (walk_direction > 0) then
        ReleaseKey("w")
    else
        ReleaseKey("s")
    end
    math.randomseed(GetRunningTime())
    walk_direction = math.random(0, 1) -- 变更行走方向
    if (strafe_direction > 0) then
        ReleaseKey("d")
    else
        ReleaseKey("a")
    end
    math.randomseed(GetRunningTime())
    strafe_direction = math.random(0, 1) -- 变更扫射方向
end

function use_skill() -- 使用角色主动技能（如果有主动技能）
    choose_class() -- 买枪之前先选择角色（再次减小选择角色与购买武器冲突的可能性，防止选错角色）
    if (GetRunningTime() % 10 == 0) then
        PressAndReleaseKey("7") -- 使用角色技能（10%概率）
        Sleep(LONG_INTERVAL)
    end
end

function combine()
    while (IsKeyLockOn("scrolllock")) do
        PressKey("enter")
        MoveMouseTo(AUTO_FILL_X, AUTO_FILL_Y)
        PressAndReleaseMouseButton(1)
        Sleep(10)
        MoveMouseTo(COMBINE_X, COMBINE_Y)
        PressAndReleaseMouseButton(1)
        Sleep(10)
        counter = counter + 1;
        if (counter > 30) then
            MoveMouseTo(RESET_X, RESET_Y)
            PressAndReleaseMouseButton(1)
            counter = 0
            Sleep(10)
        end
        ReleaseKey("enter")
    end
end

function purchase()
    x, y = GetMousePosition()
    while (IsKeyLockOn("scrolllock")) do
        MoveMouseTo(x, y) -- 兑换按钮位置
        PressAndReleaseMouseButton(1)
        Sleep(10)

        MoveMouseTo(22949, 39479) -- 选项
        PressAndReleaseMouseButton(1)
        Sleep(10)

        MoveMouseTo(35858, 42212) -- 购买按钮
        PressAndReleaseMouseButton(1)
        Sleep(10)

        MoveMouseTo(36712, 39357) -- 确认
        PressAndReleaseMouseButton(1)
        Sleep(500)

        PressAndReleaseKey("escape")

    end
end
