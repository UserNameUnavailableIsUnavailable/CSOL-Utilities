-- 游戏内各种按钮坐标
Setting = {
    -- “开始游戏”按钮
    GAMESTART_X = 47025,
    GAMESTART_Y = 42030,
    -- 游戏结算页面的“确认”按钮
    GAME_SETTLEMENT_CONFIRM_X = 44976,
    GAME_SETTLEMENT_CONFIRM_Y = 51748,
    -- ESC菜单的“取消”按钮
    ESC_MENU_CANCEL_X = 32375,
    ESC_MENU_CANCEL_Y = 43913,
    -- 【灾变】资金不足，无法购买
    GAME_INSUFFIENT_FUNDS_CONFIRM_X = 32750,
    GAME_INSUFFIENT_FUNDS_CONFIRM_Y = 34924,
    -- 【灾变】死亡状态预购买菜单“重复购买”按钮（不建议点击取消购买，会与大厅界面其他按钮位置冲突）
    GAME_PREORDER_MENU_REBUY_X = 31692,
    GAME_PREORDER_MENU_REBUY_Y = 43123,
    -- 【制造所】自动添加配件按钮
    CRAFT_PARTS_AUTO_FILL_X = 20012,
    CRAFT_PARTS_AUTO_FILL_Y = 50533,
    -- 【制造所】配件“开始合成”按钮
    CRAFT_PARTS_COMBINE_X = 25989,
    CRAFT_PARTS_COMBINE_Y = 50654,
    -- 【制造所】配件“清空”按钮
    CRAFT_PARTS_CLEAR_X = 33263,
    CRAFT_PARTS_CLEAR_Y = 50594,
    -- 【商店】点击兑换后，弹窗中的兑换物品选项
    STORE_BUY_OPTION_X = 22949,
    STORE_BUY_OPTION_Y = 39479,
    -- 【商店】物品“兑换”->“兑换”按钮（点击兑换后弹出界面上的“兑换”按钮）
    STORE_BUY_BUY_X = 36644,
    STORE_BUY_BUY_Y = 41726,
    -- 【商店】物品“兑换”->“兑换”->“确认”按钮
    STORE_BUY_BUY_CONFIRM_X = 37019,
    STORE_BUY_BUY_CONFIRM_Y = 38993
}
----- 延时设定 -----
local Delay = {
    MINI = 10,
    SHORT = 50,
    NORMAL = 100,
    MEDIUM = 150,
    LONG = 200,
    LONG_LONG = 500,
    REFRESH = 1000,
}
----- 键盘按键设定 -----
local Keyboard = {
    TILDE = "tilde",
    ESCAPE = "escape",  ONE = "1", TWO = "2", THREE = "3", FOUR = "4", FIVE = "5", SIX = "6", SEVEN = "7",
    EIGHT = "8", NINE = "9", ZERO = "0", MINUS = "-", EQUAL = "=", BACKSPACE = "backspace", TAB = "tab",
    Q = "q", W = "w", E = "e", R = "r", T = "t", Y = "y", U = "u", I = "i", O = "o",
    P = "p", LBRACKET = "lbracket", RBRACKET = "rbracket", BACKSLASH = "backslash", 
    A = "a", S = "s", D = "d", F = "f", G = "g", H = "h", J = "j", K = "k", L = "l", SEMICOLON = "semicolon",
    QUOTE = "quote", ENTER = "enter", NON_US_LAYOUT_SLASH = "non_us_slash", 
    Z = "z", X = "x", C = "c", V = "v", B = "b", N = "n", M = "m", COMMA = "comma", PERIOD = "period", SLASH = "slash",
    LSHIFT = "lshift", RSHIFT = "rshift",
    LWIN = "lgui", RWIN = "rgui",
    LALT = "lalt", RALT = "ralt",
    LCTRL = "lctrl", RCTRL = "rctrl",
    SPACE = "spacebar",
    APPS = "appkey",
    INSERT = "insert", HOME = "home", PAGEUP = "pageup",
    DELETE = "delete", END = "end", PAGEDOWN = "pagedown",
    UP = "up", LEFT = "left", DOWN = "down", RIGHT = "right",
    NUM_SLASH = "numslash", NUM_MINUS = "numminus",
    NUM_SEVEN = "num7", NUM_EIGHT = "num8", NUM_NINE = "num9", NUM_PLUS = "numplus",
    NUM_FOUR = "num4", NUM_FIVE = "num5", NUM_SIX = "num6",
    NUM_ONE = "num1", NUM_TWO = "num2", NUM_THREE = "num3", NUM_ZERO = "num0", NUM_PERIOD = "numperiod",
    NUM_ENTER = "numenter",
    PRINTSCREEN = "printscreen", PAUSE = "pause",
    CAPS_LOCK = "capslock", NUM_LOCK = "numlock", SCROLL_LOCK = "scrolllock",
    F1 = "f1", F2 = "f2", F3 = "f3", F4 = "f4", F5 = "f5", F6 = "f6",
    F7 = "f7", F8 = "f8", F9 = "f9", F10 = "f10", F11 = "f11", F12 = "f12",
    F13 = "f13", F14 = "f14", F15 = "f15", F16 = "f16", F17 = "f17", F18 = "f18",
    F19 = "f19", F20 = "f20", F21 = "f21", F22 = "f22", F23 = "f23", F24 = "f24"
}
----- 鼠标按钮设定 -----
local Mouse = {
    LEFT = 1, MIDDLE = 2 ,RIGHT = 3, X1 = 4, X2 = 5
}

local Console = {}
local Weapon = {
    PRIMARY = Keyboard.ONE,-- 主武器
    SECONDARY = Keyboard.TWO, -- 副武器
    MELEE = Keyboard.THREE, -- 近身武器
    GRENADE = Keyboard.FOUR, -- 手雷
    name = '', -- 武器名
    DONT_USE = 0, -- 不使用该武器
    number = 0, -- 第几号武器栏
    purchaseKeySeq = {}, -- 武器购买按键序列
    switchDelay = Delay.MEDIUM, -- 武器切枪延迟
    attackButton = Mouse.LEFT -- 攻击按键
}
local Player = {}
local Round = {}
local Runtime = {}
local Utility = {}
local Store = {}
local Craft = {}

-- 键盘按下但未释放的按键，true表示按键按下，nil表示弹起
Keyboard.unreleased = {}

-- 按下某个按键，不回弹
-- @param key 按下的按键，如Keyboard.ESCAPE
-- @param [delay] 按下按键之后的延迟时间，单位为毫秒，可以直接使用预定义于Delay表中的字段，如Delay.NORMAL
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Keyboard:press(key, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    PressKey(key)
    self.unreleased[key] = true
    if (type(delay) == "number")
    then
        Runtime:sleep(delay)
    end
end

-- 释放按键
-- @param key 按键名称，如Keyboard.ESCAPE
-- @param [delay] 按下按键之后的延迟时间，单位为毫秒，可以直接使用预定义于Delay表中的字段，如Delay.NORMAL
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Keyboard:release(key, delay)
    if (Runtime.exit)
    then
        return
    end
    ReleaseKey(key)
    self.unreleased[key] = nil
    if (type(delay) == "number")
    then
        Runtime:sleep(delay)
    end
end

-- 按下，而后释放按键（视为一次点击）
-- @param key 按键名称，如Keyboard.ESCAPE
-- @param [delay] 点击按键之后的延迟时间，单位为毫秒，可以直接使用预定义于Delay表中的字段，如Delay.NORMAL
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Keyboard:click(key, delay)
    if (Runtime.exit)
    then
        return
    end
    PressAndReleaseKey(key)
    if (type(delay) == "number")
    then
        Runtime:sleep(delay)
    end
end

-- 释放所有按键
-- @param [delay] 每释放一个按键之后的延迟时间，单位为毫秒，可以直接使用预定义于Delay表中的字段，如Delay.NORMAL
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Keyboard:releaseAll(delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    for key, _ in pairs(self.keyState)
    do
        self:release(key, delay)
        self.unreleased[key] = false
    end
end

-- 点击某个按键若干次
-- @param key 按键名称，如Keyboard.ESCAPE
-- @param times 重复次数
-- @param [delay] 每释放一个按键之后的延迟时间，单位为毫秒，可以直接使用预定义于Delay表中的字段，如Delay.NORMAL
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Keyboard:clickSeveralTimes(key, times, delay)
    if (Runtime.exit)
    then
        return
    end
    while (times > 0)
    do
      self:click(key, delay)
      times = times - 1
    end
end

-- keyLockState为3 bits无符号整数，大小范围：0~7，从高位到低位依次表示NUMLOCK，CAPSLOCK，SCROLLLOCK
---   2               1             0
--- NUM_LOCK       CAPS_LOCK     SCROLL_LOCK
-- 以上排布对应于键盘上指示灯排布，可表示8种状态
-- 例如， keylock = 5 = 0b101，代表NUMLOCK和SCROLLLOCK打开
Keyboard.NUMLOCK_ON = 0x4
Keyboard.CAPSLOCK_ON = 0x2
Keyboard.SCROLLLOCK_ON = 0x1
-- 获取keyLockState值
-- @return number (1~7)
function Keyboard:getKeyLockState()
    local keyLockState = 0
    if (IsKeyLockOn(self.NUM_LOCK))
    then
        keyLockState = keyLockState | self.NUMLOCK_ON
    else
        keyLockState = keyLockState & ~self.NUMLOCK_ON
    end
    if (IsKeyLockOn(self.CAPS_LOCK))
    then
        keyLockState = keyLockState | self.CAPSLOCK_ON
    else
        keyLockState = keyLockState & ~self.CAPSLOCK_ON
    end
    if (IsKeyLockOn(self.SCROLL_LOCK))
    then
        keyLockState = keyLockState | self.SCROLLLOCK_ON
    else
        keyLockState = keyLockState & ~self.SCROLLLOCK_ON
    end
    return keyLockState
end

-- 判断修饰键（如CTRL，ALT等）是否按下
-- @return boolean
function Keyboard:modifierPressed(key)
    return IsModifierPressed(key)
end

-- 鼠标按下但未释放的按钮，true表示按下，nil表示弹起
Mouse.unreleased = {}

-- 按下某个按钮，但不释放
-- @param button 按钮值，如Mouse.LEFT
-- @param [delay] 按下某个按钮后的延迟时间
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Mouse:press(button, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    PressMouseButton(button)
    self.unreleased[button] = true
    if (type(delay) == "number")
    then
        Runtime:sleep(delay)
    end
end

-- 释放某个按钮
-- @param button 按钮值，如Mouse.LEFT
-- @param [delay] 释放某个按钮后的延迟时间
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Mouse:release(button, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    ReleaseMouseButton(button)
    self.unreleased[button] = nil
    if (type(delay) == "number")
    then
        Runtime:sleep(delay)
    end
end

-- 释放所有通过Mouse:press按下但未回弹的按钮（记录在Mouse.unreleased中）
-- @param [delay] 释放每个按钮后的延迟时间
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Mouse:releaseAll(delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    for button, _ in pairs(self.unreleased)
    do
        ReleaseMouseButton(button)
        self.unreleased[button] = nil
        Runtime:sleep(delay)
    end
end

-- 单击一次按钮
-- @param button 按钮值，如Mouse.LEFT
-- @param [delay] 点击某个按钮后的延迟时间
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Mouse:click(button, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    PressAndReleaseMouseButton(button)
    if (type(delay) == "number")
    then
        Runtime:sleep(delay)
    end
end

-- 双击时间间隔150ms（人手实际双击间隔时间一般为此值）
Mouse.DOUBLE_CLICK_INTERVAL = Delay.MEDIUM 

-- 双击一次按钮
-- @param button 按钮值，如Mouse.LEFT
-- @param [delay] 双击某个按钮后的延迟时间
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Mouse:doubleClick(button, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    PressAndReleaseMouseButton(button)
    Sleep(self.DOUBLE_CLICK_INTERVAL)
    PressAndReleaseMouseButton(button)
    if (type(delay) == "number")
    then
        Runtime:sleep(delay)
    end
end

-- 使用鼠标单击屏幕上某个位置
-- @param x 横坐标
-- @param y 纵坐标
-- @return nil
-- @remark 坐标(x, y)获取方式参见Mouse:locate 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Mouse:clickOn(x, y)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    Mouse:moveTo(x, y, Delay.SHORT)
    Mouse:click(Mouse.LEFT, Delay.SHORT)
end

-- 使用鼠标双击屏幕上某个位置
-- @param x 横坐标
-- @param y 纵坐标
-- @return nil
-- @remark 坐标(x, y)获取方式参见Mouse:locate 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Mouse:doubleClickOn(x, y)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    Mouse:moveTo(x, y, Delay.SHORT)
    Mouse:doubleClick(Mouse.LEFT, Delay.SHORT)
end

-- 判断某个按钮是否按下
-- @param button 按钮值，如Mouse.LEFT
-- @return nil
function Mouse:pressed(button)
    return IsMouseButtonPressed(button)
end

-- 获取鼠标光标位置
-- @param nil
-- @return x, y 对应于横、纵坐标
function Mouse:locate()
    return GetMousePosition()
end

-- 移动鼠标光标到某位置
-- @param x 横坐标
-- @param y 纵坐标
-- @param [delay] 移动鼠标光标后的延迟时间
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Mouse:moveTo(x, y, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    MoveMouseTo(x, y)
    if (type(delay) == "number")
    then
        Runtime:sleep(delay)
    end
end

-- 相对移动鼠标光标
-- @param rightward 向右移动的偏移量，负数表示向左
-- @param downward 向下移动的偏移量，负数表示向上
-- @param [delay] 移动光标后的延迟
-- @return nil
-- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
function Mouse:moveRelative(rightward, downward, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    MoveMouseRelative(math.ceil(rightward), math.ceil(downward))
    if (type(delay) == "number")
    then
        Runtime:sleep(delay)
    end
end

-- 在控制台上打印字符串，不换行
-- @param str
-- @return nil
function Console:print(str)
    OutputLogMessage(str)
end

-- 在控制台上打印字符串，并换行
-- @param str
-- @return nil
function Console:println(str)
    OutputLogMessage(str .. '\n')
end

-- 在控制台上按照固定格式打印信息
-- @param str
-- @return nil
function Console:infomation(str)
    self:println("[INFO] " .. str)
end

-- 在控制台上按照固定格式打印错误信息
-- @param str
-- @return nil
function Console:error(str)
    self:println("[ERR] " .. str)
end

-- 清空控制台
-- @param nil
-- @return nil
function Console:clear()
    ClearLog()
end

-- 封装了math.random函数，每次调用时以运行时间为随机种子产生随机数
-- @param 同math.random
-- @remark 参见math.random了解参数信息
function Utility:generateRandom(...)
    math.randomseed(Runtime:execTime())
    return math.random(...)
end

-- 返回一个（循环）计数器
-- @param [from] 计数起始
-- @param [to]   计数终止
-- @remark 如果未提供from参数，则计数器从0开始递增计数；to为循环计数器上限，达到上限后计数器回到from（wrap-around），若不提供to参数则不设上限
function Utility:counter(from, to)
    from = from or 0
    local counter = from
    return function ()
        local ret = counter
        counter = counter + 1
        if (counter > to)
        then
            counter = from
        end
        return ret
    end
end

-- 获取日期
-- @param 同os.date
-- @remark 参见os.date了解参数信息
function Utility:date(...)
    return GetDate(...)
end

-- 运行时退出位，若置为true，则所有挂起、IO操作都将被跳过
Runtime.exit = false
-- 中断标志位，用于屏蔽中断以禁用中断嵌套
Runtime.interuptFlag = false

-- 获取当前程序运行时间，单位为毫秒
-- @param nil
-- @return nil
function Runtime:execTime()
    return GetRunningTime()
end

-- 挂起当前线程，挂起后，可以处理中断事件
-- @param duration 挂起的时间，不保证挂起的时间是否精确，尤其是有中断事件需要处理时
-- @remark 除了Runtime内部方法外，其他地方都应当调用Runtime:sleep，而非直接调用罗技API中的Sleep，这样可以进行中断处理
function Runtime:sleep(duration)
    -- 罗技API不支持真正的中断，故而当某个过程主动将自己挂起时（即调用Runtime:sleep)视为自发中断，此时可以处理外部事件
    if (self.exit) -- 下达退出命令后，挂起操作不被允许，直接返回
    then
        return
    end
    -- 中断处理
    self:interuptHandler()
    Sleep(duration)
end

-- 中断处理函数
-- @param nil
-- @return nil
-- @remark 只有中断标志位失能时才允许中断，允许中断时，按下右Ctrl、右Alt、右Shift触发中断
function Runtime:interuptHandler()
    if (self.interuptFlag) -- 未关中断才会触发中断
    then
        return
    end
    -- 右alt、右ctrl、右shift同时按下触发中断
    if (not (Keyboard:modifierPressed(Keyboard.RALT) and Keyboard:modifierPressed(Keyboard.RCTRL) and Keyboard:modifierPressed(Keyboard.RSHIFT))) 
    then
        return
    end
    -- 中断开始时，中断标志位使能以屏蔽中断
    self.interuptFlag = true
    -- 中断现场（临时变量，自动回收，以免后患）
    local interuptContext = {
            keyboardUnreleased = {},
            mouseUnreleased = {}
        }
    -- 现场切换，保护中断现场
    self:protectContext(interuptContext)
    while (true)
    do
        Sleep(Delay.LONG)
        -- 锁定值为0（全不亮），左Ctrl、左Alt、左Shift按下（表示确认），则退出位使能，此时不会恢复中断现场，任何后续的挂起、外围设备操作将直接返回，资源随之释放
        if (Keyboard:getKeyLockState() == 0 and
        (Keyboard:modifierPressed(Keyboard.LALT) and Keyboard:modifierPressed(Keyboard.LCTRL) and Keyboard:modifierPressed(Keyboard.LSHIFT)))
        then
            Runtime.exit = true
            break
        -- 锁定值为7（全亮），左Ctrl、左Alt、左Shift按下（表示确认），则撤销中断处理并恢复中断现场，程序执行不受到影响
        elseif (Keyboard:getKeyLockState() == 7 and
        (Keyboard:modifierPressed(Keyboard.LALT) and Keyboard:modifierPressed(Keyboard.LCTRL) and Keyboard:modifierPressed(Keyboard.LSHIFT))) 
        then
            -- 恢复现场
            self:restoreContext(interuptContext)
            break
        end
        -- 中断处理完毕，中断标志位失能
        self.interuptFlag = false
    end
end

-- 保护中断现场：把所有按下但未释放的键盘按键和鼠标按钮全部弹起，避免干扰中断处理，随后可以进行上下文切换
-- @param (table)interuptContext 将中断现场保存到表中
-- @return nil
function Runtime:protectContext(interuptContext)
    for key, _ in pairs(Keyboard.unreleased)
    do
        Keyboard:release(key)
        interuptContext.keyboardUnreleased[key] = true
    end
    for button, _ in pairs (Mouse.unreleased)
    do
        Mouse:release(button)
        interuptContext.mouseUnreleased[button] = true
    end
end

-- 恢复中断现场：中断处理完成后，重新按下保存中断现场中的按下但未弹起的键盘按键和鼠标按钮
-- @param (table)interuptContext 中断现场
-- @return nil
function Runtime:restoreContext(interuptContext)
    for key, _ in pairs(interuptContext.keyboardUnreleased)
    do
        Keyboard:press(key)
        interuptContext.keyboardUnreleased[key] = nil
    end
    for button, _ in pairs(interuptContext.mouseUnreleased)
    do
        Mouse:press(button)
        interuptContext.mouseUnreleased[button] = nil
    end
end

-- 阻塞等待某个流程执行完毕
-- @param (function)procedure 需要等待的流程
-- @param ... procedure接收的参数
-- @return 与procedure相同
function Runtime:waitFor(procedure, ...)
    if (type(procedure) == "function")
    then
        return procedure(...)
    end
end

-- 创建一个武器对象
-- @param (table)[obj] 初始化列表
-- @return table 武器对象
function Weapon:new(obj)
    obj = obj or {}
    self.__index = self
    setmetatable(obj, self)
    Console:infomation(
        "新增武器：" .. obj.name
    )
    return obj
end

-- 根据purchaseKeySeq字段中预设的按键购买武器
-- @param nil
-- return nil
function Weapon:purchase()
    for _, key in ipairs(self.purchaseKeySeq)
    do
        Keyboard:click(key, Delay.LONG)
    end 
end
function Weapon:abandon()
    Player:swtichWeaponTo(self)
    Keyboard:click(Keyboard.G, Delay.NORMAL)
end
-- 确认武器购买资金不足提示框（预设按钮在Setting中）
-- @param nil
-- @return nil
function Weapon:inCasePurchaseFailsForInsuffFunds()
    Mouse:clickOn(Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_X, Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_Y)
    Keyboard:click(Keyboard.ZERO, Delay.SHORT)
end

-- 关闭死亡状态下的预购买菜单（点击“重复购买”按钮，不点击“取消购买”以避免与大厅界面按钮冲突）
-- @param nil
-- @return nil
function Weapon:closePreorderMenu()
    Mouse:clickOn(Setting.GAME_PREORDER_MENU_REBUY_X, Setting.GAME_PREORDER_MENU_REBUY_Y)
end

-- 开始使用该武器攻击
-- @param nil
-- @return nil
function Weapon:startAttack()
    Mouse:press(self.attackButton)
end

-- 停止使用该武器攻击
-- @param nil
-- @return nil
function Weapon:stopAttack()
    Mouse:release(self.attackButton)
end

-- 跑动方向，0表示向前，1表示向后
Player.runDirection = 1
-- 扫射方向，0表示向右，1表示向左
Player.strafeDirection = 1

-- 开始按照随机的方向Player.runDirection和Player.strafeDirection移动
-- @param nil
-- @return nil
function Player:startMove()
    self.runDirection = Utility:generateRandom(0, 1)
    self.strafeDirection = Utility:generateRandom(0, 1)
    if (self.runDirection == 0)
    then
        Keyboard:press(Keyboard.W, Delay.NORMAL)
    else
        Keyboard:press(Keyboard.S, Delay.NORMAL)
    end
    if (self.strafeDirection == 0)
    then
        Keyboard:press(Keyboard.D, Delay.NORMAL)
    else
        Keyboard:press(Keyboard.A, Delay.NORMAL)
    end
end

-- 结束移动
-- @param nil
-- @return nil
function Player:stopMove()
    if (self.runDirection == 0)
    then
        Keyboard:release(Keyboard.W, Delay.NORMAL)
    else
        Keyboard:release(Keyboard.S, Delay.NORMAL)
    end
    if (self.strafeDirection == 0)
    then
        Keyboard:release(Keyboard.D, Delay.NORMAL)
    else
        Keyboard:release(Keyboard.A, Delay.NORMAL)
    end
end

-- Player对象
-- 一次游戏中可以创建多个Player对象，每个对象可以使用不同的武器序列，按照不同的方式移动，一切根据自己需要来改写Player中的方法，或者提供新方法

-- 一些概念说明如下
-- 主要武器chief weapon：用来攻击的武器，如魔法刀
-- 配件武器part weapon：配件武器，不用于攻击
-- 辅助武器auxiliary weapon：可与主要武器并行攻击，如天使雷
-- 说明
-- 对于主要武器（无特殊攻击，如神鬼左R），只需要按照如下所示的方式创建三张武器列表
-- 对于配件武器，按照如下方式创建武器列表即可
-- 对于辅助武器，需要自行实现使用辅助武器的函数Player：useAuxiliaryWeapon
-- 推荐做法
-- 对于特殊攻击方式的武器，在创建武器对象时，除继承Weapon类的方法外，应当重写其中与攻击有关的方法，
-- 或者可以单独提供攻击的方法（见Player：auxiliaryWeapon创建示例，其中提供了use函数，use函数在Player:useAuxiliaryWeapon被调用）
-- 并对应地自行实现Player：useChiefWeapon或Player.useAuxliaryWeapon

-- 使用的默认主要武器序列
-- 强烈建议主要武器序列中第一件武器为带恢复配件武器（如魔法刀），且其购买按键序列不包含数字，原因如下：

-- 本程序照如下流程执行：
--   ------------------------------------------------------------------------------
--   |                                                                             |
--   \/                                                                            |
-- [ 开 始 游 戏 ]->[ 重 置 状 态 (每隔一段时间发生一次) ]->[ 正常购买、攻击流程 ]------
--                    等待加载
--                    购买默认武器
--                    攻击
--                    选择人物
--                
-- 当程序开始运行并加载进入游戏时，会在一段时间内（重置状态）重复购买主要武器序列中的第一件武器，且在这段时间内仅用该武器攻击，然后执行选择人物的操作。
-- 每局游戏的时间较长，且时间根据进入游戏的时刻不同变化很大。因此为了尽快进入游戏（尤其是房主，全部准备完毕后等待时间超过60秒强制被踢；对于一般玩家，长时间不进入游戏也会被踢），
-- 将重置状态发生的条件设定为每隔一段固定的较短时间（如一分钟）触发一次。这样，每隔一分钟，就会执行点击“开始游戏”按钮的操作，
-- 点击“开始游戏”后即认为游戏正在加载进度条（保守起见预设为20秒，实际一般15秒左右），但是实际情况很可能是是一局游戏正在进行，因此这段加载时间不能什么都不做，必须要使用某件武器
-- 进行攻击。故此，在这20秒的时间间隔内，仅购买并使用默认的主要武器（chiefWeapon武器列表的第一项武器）攻击。
-- 当20秒时间间隔结束，执行选择人物操作。如果之前默认的主要武器购买按键序列中包含数字，就会导致选错人物。
-- 另外，考虑到真正游戏开局血量、资金等问题，也强烈建议将第一件武器设置成恢复配件的近战武器。
Player.chiefWeapon = {
    Weapon:new({
        name = "幻境！光棱剑",
        switchDelay = Delay.NORMAL,
        number = Weapon.MELEE,
        purchaseKeySeq = {Keyboard.B, Keyboard.G},
        attackButton = Mouse.LEFT})
}

-- 使用的默认配件武器序列
Player.partWeapon = {
    Weapon:new({
        name = "星战前线·加特林",
        number = Weapon.DONT_USE,
        purchaseKeySeq = {Keyboard.B, Keyboard.J, Keyboard.TWO, Keyboard.FOUR}}),
    Weapon:new({
        name = "FNP-45战损版",
        number = Weapon.DONT_USE,
        purchaseKeySeq = {Keyboard.B, Keyboard.J, Keyboard.ONE, Keyboard.TWO}})
}

-- 使用的默认辅助武器序列
Player.auxiliaryWeapon = {
    Weapon:new({
        name = "圣翼皓印",
        switchDelay = Delay.LONG_LONG,
        number = Weapon.GRENADE ,
        purchaseKeySeq = {Keyboard.H, Keyboard.NINE},
        discharging = false,
        turnOnMoment = -114514,
        turnOffMoment = -114514,
        DISCHARGE_TIME = 24 * 1000,
        RECHARGE_TIME = 9 * 1000,
        use = function (self)
            if (not self.discharging and Runtime:execTime() - self.turnOffMoment > self.RECHARGE_TIME)
            then
                self.discharging = true
                self.turnOnMoment = Runtime:execTime()
                Player:swtichWeaponTo(self)
                Mouse:click(Mouse.LEFT, Delay.NORMAL)
            elseif (self.discharging and Runtime:execTime() - self.turnOnMoment > self.DISCHARGE_TIME)
            then
                self.discharging = false
                self.turnOffMoment = Runtime:execTime()
                Player:swtichWeaponTo(self)
                Mouse:moveRelative(0, 4000, Delay.NORMAL)
                Keyboard:click(Keyboard.R, self.switchDelay)
                Mouse:moveRelative(0, -4000, Delay.NORMAL)
            end
        end
    })
}

-- 护甲
Player.AC = 
    Weapon:new({
        name = "避弹衣+头盔",
        purchaseKeySeq = {Keyboard.H, Keyboard.TWO},
        number = 0
    })

-- 进入正常游戏状态（非重置状态）后，按照下面概率分布，随机地购买主要武器；对于配件和辅助武器，采用循环购买方式，如欲使用随机购买可仿照主要武器随机购买的例程自行实现。
-- 概率分布相关知识请参见概率论，不予赘述。
-- 主要武器的购买概率分布，项数要与主要武器列表项数匹配，由于主要武器列表只有一件武器，仅有一项1（后面还会展示如何创建多项，并创建多个Player实例）
Player.chiefWeaponProbabilityDistribution = { 1 }

-- 每次购买完成后，会将下面要使用的武器设置成刚刚买好的武器
Player.chiefWeaponToUse = Player.chiefWeapon[1]
Player.auxiliaryWeaponToUse = Player.auxiliaryWeapon[1]

-- 是否仅使用默认武器，在重置状态下使用该字段
Player.useDefaultWeapon = false

-- nextPartWeaponToBuy是一个计数器（在Player:purchasePartWeapon中进行初始化），每次调用该计数器返回一个整数值
-- 该整数值代表了Player.partWeapon表的下标
Player.nextPartWeaponToBuy = nil

-- 按照次序，根据计数器返回值循环地购买配件武器
-- @param nil
-- @return nil
function Player:purchasePartWeapon()
    if (not self.partWeapon or #self.partWeapon == 0)
    then
        return
    end
    if (not self.nextPartWeaponToBuy) -- 计数器尚未被初始化，则创建一个赋给nextPartWeaponToBuy
    then
        self.nextPartWeaponToBuy = Utility:counter(1, #self.partWeapon) 
    end
    self.partWeapon[self:nextPartWeaponToBuy()]:purchase() -- 调用计数器函数获取下一件要购买和使用的武器
end

-- 购买辅助武器（准备使用该武器），同上
Player.nextAuxiliaryWeaponToBuy = nil
-- 按照次序，根据计数器返回值循环地购买辅助武器，并在下次使用该武器
-- @param nil
-- @return nil
function Player:purchaseAuxiliaryWeapon()
    local weapon
    if (not self.auxiliaryWeapon or #self.auxiliaryWeapon == 0)
    then
        return
    end
    if (not self.nextAuxiliaryWeaponToBuy)
    then
        self.nextAuxiliaryWeaponToBuy = Utility:counter(1, #self.auxiliaryWeapon)
    end
    weapon = self.auxiliaryWeapon[self.nextAuxiliaryWeaponToBuy()]
    weapon:purchase()
    self.auxiliaryWeaponToUse = weapon
    self:swtichWeaponTo(self.chiefWeaponToUse)
end

-- 随机购买主要武器，并在下次使用该武器
-- @param nil
-- @return nil
function Player:purchaseChiefWeapon()
    if (self.useDefaultWeapon or #self.chiefWeapon == 1) -- 默认武器位使能，或武器列表只有一件武器
    then
        self.chiefWeaponToUse = self.chiefWeapon[1]
    else
        local weapon
        local random
        random = Utility:generateRandom()
        repeat
            for i = 1, #self.chiefWeaponProbabilityDistribution
            do
                if (random < self.chiefWeaponProbabilityDistribution[i]) -- 根据概率分布确定被随机到的武器
                then
                    weapon = self.chiefWeapon[i]
                    break
                end
            end
        until weapon.name ~= self.chiefWeaponToUse.name
        weapon:purchase()
        self.chiefWeaponToUse = weapon
    end
    self:swtichWeaponTo(self.chiefWeaponToUse)
end

-- 使用辅助武器
function Player:useAuxiliaryWeapon()
    self.auxiliaryWeaponToUse:use()
end

-- 购买护甲
function Player:purchaseAC()
    self.AC:purchase()
end

-- 随机使用血包
-- @param [(table)probabilityDistribution] 概率分布表
-- @return nil
-- @remark 依照概率分布随机使用血包（5和6），故分布表应有两项
function Player:randomlyUseMedicalKits(probabilityDistribution)
    local probabilityDistribution = probabilityDistribution or {0.1, 0.15}
    local random = Utility:generateRandom()
    if (random < probabilityDistribution[1])
    then
        Keyboard:press(Keyboard.FIVE, Delay.SHORT)
    elseif (random < probabilityDistribution[2])
    then
        Keyboard:press(Keyboard.SIX, Delay.SHORT)
    end
end

-- 创建Player实例
-- @param [obj] 初始化列表
-- @return nil
-- @remark 如果选择直接创建Player实例，则默认使用类中预定义好的若干内容
function Player:new(obj)
    self.__index = self
    obj = obj or {}
    setmetatable(obj, self)
    return obj
end

-- 切换到指定武器
-- @param weapon 武器对象
-- @return nil
function Player:swtichWeaponTo(weapon)
    Keyboard:click(weapon.number, weapon.switchDelay)
end

-- 一次转圈的时长
Player.turnDuration = 5.5 * 1000

-- 以随机方向、随机速度转圈
-- @param nil
-- @return nil
function Player:turn()
    local rotateStopMoment = Runtime:execTime() + self.turnDuration
    local sensitivity = (2 - Utility:generateRandom()) / 2 -- 灵敏度∈(0.5, 1]
    local direction
    if (Utility:generateRandom(0, 1) == 0)
    then
        direction = 1
    else
        direction = -1
    end
    repeat
        Mouse:moveRelative(100 * direction * sensitivity, 0, Delay.MINI)
    until Runtime:execTime() > rotateStopMoment
    Keyboard:click(Keyboard.R, Delay.SHORT)
end

function Player:startAttack()
    self.chiefWeaponToUse:startAttack()
end

function Player:stopAttack()
    self.chiefWeaponToUse:stopAttack()
end

-- 一局时间（非游戏中一局真实时间），原因不再赘述
Round.ROUND_TIME = 60 * 1000
-- 加载进度条时间
Round.LOAD_TIME = 20 * 1000
-- 玩家列表
Round.playerList = {}
Round.playerNumber = 1
-- 使用的虚拟玩家

-- 初始化，创建玩家
-- @param nil
-- @return nil
function Round:initialize()
    Round.timer = -114514
    self.defaultPlayer = Player:new()
    self.player = Player:new({
        chiefWeapon = {
            Weapon:new{
                name = "幻境！光棱剑",
                switchDelay = Delay.NORMAL,
                number = Weapon.MELEE,
                purchaseKeySeq = {Keyboard.B, Keyboard.G},
                attackButton = Mouse.LEFT
            },
            Weapon:new{
                name = "【擎空】突击套装",
                switchDelay = Delay.NORMAL,
                number = Weapon.SECONDARY,
                purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.ONE},
                attackButton = Mouse.LEFT
            },
            Weapon:new{
                name = "【天驱】突击套装",
                switchDelay = Delay.NORMAL,
                number = Weapon.SECONDARY,
                purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.TWO},
                attackButton = Mouse.LEFT
            },
            Weapon:new{
                name = "冰雪魔咒PB-4",
                switchDelay = Delay.NORMAL,
                number = Weapon.SECONDARY,
                purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.THREE},
                attackButton = Mouse.LEFT
            },
            Weapon:new{
                name = "次元裁决",
                switchDelay = Delay.NORMAL,
                number = Weapon.SECONDARY,
                purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.FOUR},
                attackButton = Mouse.LEFT
            },
            Weapon:new{
                name = "晖耀审判",
                switchDelay = Delay.NORMAL,
                number = Weapon.SECONDARY,
                purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.FIVE},
                attackButton = Mouse.LEFT
            },
            Weapon:new{
                name = "【深渊】Elites",
                switchDelay = Delay.SHORT,
                number = Weapon.SECONDARY,
                purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.EIGHT},
                attackButton = Mouse.RIGHT
            },
            Weapon:new{
                name = "【恶龙】霰弹炮",
                switchDelay = Delay.NORMAL,
                number = Weapon.PRIMARY,
                purchaseKeySeq = {Keyboard.B, Keyboard.TWO, Keyboard.ONE},
                attackButton = Mouse.LEFT
            },
            Weapon:new{
                name = "【异噬】暗雷兽",
                switchDelay = Delay.NORMAL,
                number = Weapon.PRIMARY,
                purchaseKeySeq = {Keyboard.B, Keyboard.TWO, Keyboard.TWO},
                attackButton = Mouse.LEFT
            },
            Weapon:new{
                name = "暗狱雷魂",
                switchDelay = Delay.NORMAL,
                number = Weapon.PRIMARY,
                purchaseKeySeq = {Keyboard.H, Keyboard.THREE},
                attackButton = Mouse.LEFT
            },
            Weapon:new{
                name = "逆界星轮",
                switchDelay = Delay.NORMAL,
                number = Weapon.PRIMARY,
                purchaseKeySeq = {Keyboard.H, Keyboard.SIX},
                attackButton = Mouse.RIGHT
            }
        },
        partWeapon = {},
        -- 辅助武器默认使用天使雷
        -- 主要武器概率分布
        chiefWeaponProbabilityDistribution = {0.2, 0.28, 0.36, 0.44, 0.52, 0.60, 0.68, 0.76, 0.84, 0.92, 1.00}
    })
    self.playerList = {
        self.defaultPlayer,
        self.player
    }
    Console:infomation(string.format("Create %d player(s).", #self.playerList))
    self:chooseMainPlayer(true)
end

-- 重置一局，选定角色
-- @param nil
-- @return nil
function Round:reset()
    Console:infomation("Start resetting round.")
    local moment = Runtime:execTime()
    self.playerList[self.playerNumber].useDefaultWeapon = true
    repeat
        self.playerList[self.playerNumber]:purchaseChiefWeapon()
        self.playerList[self.playerNumber]:startMove()
        self.playerList[self.playerNumber]:startAttack()
        self.playerList[self.playerNumber]:turn()
        self.playerList[self.playerNumber]:stopAttack()
        self.playerList[self.playerNumber]:stopMove()
    until Runtime:execTime() - Round.LOAD_TIME > moment
    Round:chooseClass()
    self.playerList[self.playerNumber].useDefaultWeapon = false
    Console:infomation("Finish resetting round.")
end

-- 按ESC键清除所有UI
-- @param nil
-- @return nil
function Round:clearUI()
    Keyboard:clickSeveralTimes(Keyboard.ESCAPE, 2, Delay.SHORT)
    Mouse:clickOn(Setting.ESC_MENU_CANCEL_X, Setting.ESC_MENU_CANCEL_Y)
    Mouse:clickOn(Setting.GAME_SETTLEMENT_CONFIRM_X, Setting.GAME_SETTLEMENT_CONFIRM_Y) -- 点击ESC菜单的取消按钮
end

-- 角色选项
Round.classOption = Keyboard.THREE
-- 选定角色
-- @param nil
-- @return nil
function Round:chooseClass()
    for _ = 1, 3
    do
        Keyboard:click(self.classOption, Delay.SHORT)
    end
end

function Round:chooseMainPlayer(confirm)
    confirm = confirm or (Keyboard:modifierPressed(Keyboard.LCTRL) and Keyboard:modifierPressed(Keyboard.RCTRL))
    local number = Keyboard:getKeyLockState()
    if (confirm and number > 0 and number <= #self.playerList)
    then
        self.playerNumber = number
        return true
    end
    self.playerNumber = 1
    return false
end

-- 回合计时器
Round.timer = -114514
-- 开始挂机
-- @param nil
-- @return nil
function Round:start()
    self:initialize() -- 初始化
    while (not Runtime.exit)  -- 没有要求退出
    do
        if (Keyboard:getKeyLockState() == 0) -- 锁存器值为0，进入自陷状态
        then
            Console:infomation("Procedure Round:start() is suspended.")
            local exit = Runtime:waitFor(
                function()
                    while (not(Runtime.exit or self:chooseMainPlayer(false))) -- 未发出退出或者继续指令则保持挂起
                    do
                        Runtime:sleep(Delay.LONG_LONG)
                    end
                    return Runtime.exit
                end
            )
            
            if (exit) -- 退出
            then
                Console:infomation("Procedure Round:start() will exit.")
                Keyboard:releaseAll(Delay.SHORT)
                Mouse:releaseAll(Delay.SHORT)
                break
            end
            Console:infomation("Procedure Round:start() is resumed.")
        end
        if (Runtime:execTime() - Round.timer > Round.ROUND_TIME)
        then
            Mouse:clickOn(Setting.GAMESTART_X, Setting.GAMESTART_Y)
            self:reset()
            self.timer = Runtime:execTime()
        end
        -- 概率购买配件和辅助武器，防止购买次数过多
        local random = Utility:generateRandom()
        if (random <= 0.33)
        then
            self.playerList[self.playerNumber]:purchasePartWeapon()
        elseif (random <= 0.67)
        then
            self.playerList[self.playerNumber]:purchaseAuxiliaryWeapon()
        end
        self.playerList[self.playerNumber]:purchaseChiefWeapon()
        self:clearUI()
        self.playerList[self.playerNumber]:startMove()
        self.playerList[self.playerNumber]:startAttack()
        self.playerList[self.playerNumber]:turn()
        self.playerList[self.playerNumber]:stopAttack()
        self.playerList[self.playerNumber]:stopMove()
        self.playerList[self.playerNumber]:useAuxiliaryWeapon()
        self.playerList[self.playerNumber]:purchaseAC()
    end
    Console:infomation("Procedure Round:start() has exited.")
end

function Store:purchaseItems()
    local buyX, buyY = Mouse:locate() -- “兑换”按钮坐标
    Console:infomation(string.format("开始批量购买物品，兑换按钮位置：(%d, %d)", buyX, buyY))
    while (Keyboard:getKeyLockState() == 1 and not Runtime.exit)
    do
        Mouse:clickOn(buyX, buyY)
        Mouse:clickOn(Setting.STORE_BUY_OPTION_X, Setting.STORE_BUY_OPTION_Y) -- 选项
        Mouse:clickOn(Setting.STORE_BUY_BUY_X, Setting.STORE_BUY_BUY_Y) -- 二级“兑换”按钮
        Mouse:clickOn(Setting.STORE_BUY_BUY_CONFIRM_X, Setting.STORE_BUY_BUY_CONFIRM_Y) -- “确认”按钮
        Keyboard:click(Keyboard.ESCAPE, Delay.SHORT)
    end
    Console:infomation("物品购买结束")
end

function Craft:combineParts()
    local counter = 0
    Console:infomation("开始合成配件")
    while (Keyboard:getKeyLockState() == 1 and not Runtime.exit)
    do
        Keyboard:press(Keyboard.ENTER, Delay.MINI)
        Mouse:clickOn(Setting.CRAFT_PARTS_AUTO_FILL_X, Setting.CRAFT_PARTS_AUTO_FILL_Y)
        Mouse:clickOn(Setting.CRAFT_PARTS_COMBINE_X, Setting.CRAFT_PARTS_COMBINE_Y)
        counter = counter + 1
        if (counter > 30)
        then
            Mouse:clickOn(Setting.CRAFT_PARTS_CLEAR_X, Setting.CRAFT_PARTS_CLEAR_Y)
            counter = 0
        end
        Keyboard:release(Keyboard.ENTER, Delay.MINI)
    end
    Console:infomation("合成配件结束")
end

function OnEvent(event, arg, family)
    Runtime.exit = false
    if (Keyboard:getKeyLockState() > 0)
    then
        if (event == "MOUSE_BUTTON_PRESSED" and arg == 7 and family == "mouse")
        then
            Round:start()
            Mouse:moveTo(32768, 32768)
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 6 and family == "mouse")
        then
            local x, y = Mouse:locate()
            Console:infomation(string.format("Coordinate: (%d, %d)", x, y))
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 5 and family == "mouse")
        then
            Store:purchaseItems()
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 4 and family == "mouse")
        then
            Craft:combineParts()
        end
    end
end