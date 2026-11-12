local Keyboard = {}

---@enum Keyboard.KeyName
Keyboard.KeyName = {
    -- 功能区
    ESCAPE = "escape",
    F1 = "f1",
    F2 = "f2",
    F3 = "f3",
    F4 = "f4",
    F5 = "f5",
    F6 = "f6",
    F7 = "f7",
    F8 = "f8",
    F9 = "f9",
    F10 = "f10",
    F11 = "f11",
    F12 = "f12",
    F13 = "f13",
    F14 = "f14",
    F15 = "f15",
    F16 = "f16",
    F17 = "f17",
    F18 = "f18",
    F19 = "f19",
    F20 = "f20",
    F21 = "f21",
    F22 = "f22",
    F23 = "f23",
    F24 = "f24",
    -- 主区
    BACKQUOTE = "tilde",
    ONE = "1",
    TWO = "2",
    THREE = "3",
    FOUR = "4",
    FIVE = "5",
    SIX = "6",
    SEVEN = "7",
    EIGHT = "8",
    NINE = "9",
    ZERO = "0",
    MINUS = "minus",
    EQUAL = "equal",
    BACKSPACE = "backspace",
    TAB = "tab",
    Q = "q",
    W = "w",
    E = "e",
    R = "r",
    T = "t",
    Y = "y",
    U = "u",
    I = "i",
    O = "o",
    P = "p",
    LEFT_BRACKET = "lbracket",
    RIGHT_BRACKET = "rbracket",
    BACKSLASH = "backslash",
    CAPS_LOCK = "capslock",
    A = "a",
    S = "s",
    D = "d",
    F = "f",
    G = "g",
    H = "h",
    J = "j",
    K = "k",
    L = "l",
    SEMICOLON = "semicolon",
    QUOTE = "quote",
    ENTER = "enter",
    LEFT_SHIFT = "lshift",
    NON_US_BACKSLASH = "non_us_slash",
    Z = "z",
    X = "x",
    C = "c",
    V = "v",
    B = "b",
    N = "n",
    M = "m",
    COMMA = "comma",
    PERIOD = "period",
    SLASH = "slash",
    RIGHT_SHIFT = "rshift",
    LEFT_CTRL = "lctrl",
    LEFT_WIN = "lgui",
    LEFT_ALT = "lalt",
    SPACE = "spacebar",
    RIGHT_ALT = "ralt",
    RIGHT_WIN = "rgui",
    APPS = "appkey",
    RIGHT_CTRL = "rctrl",
    -- 控制区
    PRINT_SCREEN = "printscreen",
    SCROLL_LOCK = "scrolllock",
    PAUSE_BREAK = "pause",
    INSERT = "insert",
    HOME = "home",
    PAGE_UP = "pageup",
    DELETE = "delete",
    END = "end",
    PAGE_DOWN = "pagedown",
    UP = "up",
    LEFT = "left",
    DOWN = "down",
    RIGHT = "right",
    -- 数字区
    NUM_LOCK = "numlock",
    NUM_DIVIDE = "numslash",
    NUM_MINUS = "numminus",
    NUM_MULTIPLY = "numstar",
    NUM_SEVEN = "num7",
    NUM_EIGHT = "num8",
    NUM_NINE = "num9",
    NUM_PLUS = "numplus",
    NUM_FOUR = "num4",
    NUM_FIVE = "num5",
    NUM_SIX = "num6",
    NUM_ONE = "num1",
    NUM_TWO = "num2",
    NUM_THREE = "num3",
    NUM_ZERO = "num0",
    NUM_DECIMAL = "numperiod",
    NUM_ENTER = "numenter",
    -- 注意：这三个不区分左右的修饰键只能用于 `is_modifier_pressed` 中
    CTRL = "ctrl",
    SHIFT = "shift",
    ALT = "alt",
}

---@enum Keyboard.KeyCode
Keyboard.KeyCode = {
    ESCAPE = 0x01,
    F1 = 0x3b,
    F2 = 0x3c,
    F3 = 0x3d,
    F4 = 0x3e,
    F5 = 0x3f,
    F6 = 0x40,
    F7 = 0x41,
    F8 = 0x42,
    F9 = 0x43,
    F10 = 0x44,
    F11 = 0x57,
    F12 = 0x58,
    F13 = 0x64,
    F14 = 0x65,
    F15 = 0x66,
    F16 = 0x67,
    F17 = 0x68,
    F18 = 0x69,
    F19 = 0x6a,
    F20 = 0x6b,
    F21 = 0x6c,
    F22 = 0x6d,
    F23 = 0x6e,
    F24 = 0x76,
    PRINT_SCREEN = 0x137,
    SCROLL_LOCK = 0x46,
    PAUSE_BREAK = 0x146,
    INSERT = 0x152,
    DELETE = 0x153,
    HOME = 0x147,
    END = 0x14f,
    PAGE_UP = 0x149,
    PAGE_DOWN = 0x151,
    UP = 0x148,
    LEFT = 0x14b,
    DOWN = 0x150,
    RIGHT = 0x14d,
    BACKQUOTE = 0x29,
    ONE = 0x02,
    TWO = 0x03,
    THREE = 0x04,
    FOUR = 0x05,
    FIVE = 0x06,
    SIX = 0x07,
    SEVEN = 0x08,
    EIGHT = 0x09,
    NINE = 0x0a,
    ZERO = 0x0b,
    MINUS = 0x0c,
    EQUAL = 0x0d,
    BACKSPACE = 0x0e,
    TAB = 0x0f,
    Q = 0x10,
    W = 0x11,
    E = 0x12,
    R = 0x13,
    T = 0x14,
    Y = 0x15,
    U = 0x16,
    I = 0x17,
    O = 0x18,
    P = 0x19,
    LEFT_BRACKET = 0x1a,
    RIGHT_BRACKET = 0x1b,
    BACKSLASH = 0x2b,
    CAPS_LOCK = 0x3a,
    A = 0x1e,
    S = 0x1f,
    D = 0x20,
    F = 0x21,
    G = 0x22,
    H = 0x23,
    J = 0x24,
    K = 0x25,
    L = 0x26,
    SEMICOLON = 0x27,
    QUOTE = 0x28,
    ENTER = 0x1c,
    LEFT_SHIFT = 0x2a,
    NON_US_SLASH = 0x56,
    Z = 0x2c,
    X = 0x2d,
    C = 0x2e,
    V = 0x2f,
    B = 0x30,
    N = 0x31,
    M = 0x32,
    COMMA = 0x33,
    PERIOD = 0x34,
    SLASH = 0x35,
    RIGHT_SHIFT = 0x36,
    LEFT_CTRL = 0x1d,
    LEFT_WIN = 0x15b,
    LEFT_ALT = 0x38,
    SPACE = 0x39,
    RIGHT_ALT = 0x138,
    RIGHT_WIN = 0x15c,
    APPS = 0x15d,
    RIGHT_CTRL = 0x11d,
    NUM_LOCK = 0x45,
    NUM_DIVIDE = 0x135,
    NUM_MULTIPLY = 0x37,
    NUM_MINUS = 0x4a,
    NUM_SEVEN = 0x47,
    NUM_EIGHT = 0x48,
    NUM_NINE = 0x49,
    NUM_PLUS = 0x4e,
    NUM_FOUR = 0x4b,
    NUM_FIVE = 0x4c,
    NUM_SIX = 0x4d,
    NUM_ONE = 0x4f,
    NUM_TWO = 0x50,
    NUM_THREE = 0x51,
    NUM_ZERO = 0x52,
    NUM_DECIMAL = 0x53,
    NUM_ENTER = 0x11c,
}

---检查给定的按键名是否有效。
---@param key any 按键名或按键扫描码
local function IsValidKey(key)
    if type(key) == "string" then
        for _, v in pairs(Keyboard.KeyName) do
            if v == key then
                return true
            end
        end
    end
    if type(key) == "number" then
        for _, v in pairs(Keyboard.KeyCode) do
            if v == key then
                return true
            end
        end
    end
    return false
end

---通过 `press` 按下但尚未通过 `release` 释放的按键。
---@type { [Key]: boolean }
local unreleased = {}

local frozen = true

local L = require("Logitech")
L.AfterLoad(function ()
    frozen = false
end)

---Freeze keyboard operations.
function Keyboard.Freeze()
    frozen = true
end

---Unfreeze keyboard operations.
function Keyboard.UnFreeze()
    frozen = false
end

---Whether the keyboard is in frozen state.
---@return boolean
function Keyboard.IsFrozen()
    return frozen
end

---@alias Key Keyboard.KeyName|Keyboard.KeyCode|string|integer

---@param key Key|Key[] One or more key names or key codes.
---@param delay? integer 按下按键后的延迟时间。可以直接使用预定义于Delay表中的字段，如 `Delay.NORMAL`。
---@param precise? boolean|nil 是否精确定时
---按下按键。
function Keyboard.Press(key, delay, precise)
    if not Keyboard.IsFrozen() then
        if IsValidKey(key) then
            unreleased[key] = true
            L.PressKey(key--[[@as string|integer]])
        end
        if type(key) == "table" then
            local keys = {}
            for _, v in ipairs(key) do
                if IsValidKey(v) then
                    keys[#keys + 1] = v
                    unreleased[v] = true
                end
            end
            PressKey(table.unpack(keys))
        end
    end
    -- TODO: Await this call when Runtime.sleep returns a Promise.
    Runtime.sleep(delay, precise)
end

---弹起按键。
---@param key Key|Key[] 按键名称，如 `Keyboard.ESCAPE`。当 `Keyboard.IsFrozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay? integer 按下按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
---@param precise? boolean 是否精确定时
function Keyboard.Release(key, delay, precise)
    if not Keyboard.IsFrozen() then
        if type(key) == "string" and IsValidKey(key) then
            unreleased[key] = nil
            ReleaseKey(key)
        end
        if type(key) == "table" then
            local keys = {}
            for _, v in ipairs(key) do
                if IsValidKey(v) then
                    keys[#keys + 1] = v
                    unreleased[v] = nil
                end
            end
            ReleaseKey(table.unpack(keys))
        end
    end
    -- TODO: Await this call when Runtime.sleep returns a Promise.
    Runtime.sleep(delay, precise)
end

---按下，而后弹起按键（视为一次点击）。当 `Keyboard.IsFrozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param key Key|Key[] 按键名称，如 `Keyboard.ESCAPE`
---@param delay? integer 点击按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
---@param precise? boolean 是否精确定时
function Keyboard.Click(key, delay, precise)
    if not Keyboard.IsFrozen() then
        if type(key) == "string" and IsValidKey(key) then
            PressAndReleaseKey(key)
        end
        if type(key) == "table" then
            local keys = {}
            for _, v in ipairs(key) do
                if IsValidKey(v) then
                    keys[#keys + 1] = v
                end
            end
            PressAndReleaseKey(table.unpack(keys))
        end
    end
    -- TODO: Await this call when Runtime.sleep returns a Promise.
    Runtime.sleep(delay, precise)
end

---释放所有按键。
function Keyboard.reset()
    for key, _ in pairs(unreleased) do
        ReleaseKey(key)
        -- TODO: Await this call when Runtime.sleep returns a Promise.
        Runtime.sleep(Delay.MINI)
        unreleased[key] = nil
    end
end

---点击某个按键若干次。
---@param key Key 按键名称，如 `Keyboard.ESCAPE`。当 `Keyboard.IsFrozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param times? integer 重复次数。
---@param interval? integer 间隔
---@param delay? integer 操作完成后的延迟时间
---@param precise? boolean 是否精确定时
function Keyboard.click_several_times(key, times, interval, delay, precise)
    interval = interval or Delay.SHORT
    delay = delay or Delay.SHORT
    times = times or 0
    if not Keyboard.IsFrozen() and math.type(times) == "integer" and IsValidKey(key) then
        for i = 1, times do
            PressAndReleaseKey(key)
            if i ~= times then
                -- TODO: Await this call when Runtime.sleep returns a Promise.
                Runtime.sleep(interval, precise)
            end
        end
    end
    -- TODO: Await this call when Runtime.sleep returns a Promise.
    Runtime.sleep(delay, precise)
end


---通过键盘输入由字母和数字构成的字符串序列
---@param s string ASCII 字符
---@param interval? integer
---@param delay? integer
---@param precise? boolean
function Keyboard.send_input(s, interval, delay, precise)
    interval = interval or Delay.SHORT
    delay = delay or Delay.SHORT
    if not Keyboard.IsFrozen() then
        local shift = false -- shift 是否按下
        for i = 1, #s do
            local c = s:sub(i, i)
            if string.byte("0") <= string.byte(c) and string.byte(c) <= string.byte("9") then
                Keyboard.Click(c, interval, precise)
            elseif string.byte("A") <= string.byte(c) and string.byte(c) <= string.byte("Z") then
                if
                    not IsKeyLockOn(Keyboard.CAPS_LOCK) -- 非大写锁定状态
                then
                    Keyboard.Press(Keyboard.LEFT_SHIFT)
                    shift = true
                end
                Keyboard.Click(c:lower(), interval, precise)
                if shift then
                    Keyboard.Release(Keyboard.LEFT_SHIFT)
                    shift = false
                end
            elseif string.byte("a") <= string.byte(c) and string.byte(c) <= string.byte("z") then
                if
                    IsKeyLockOn(Keyboard.CAPS_LOCK) -- 大写锁定状态
                then
                    Keyboard.Press(Keyboard.LEFT_SHIFT)
                    shift = true
                end
                Keyboard.Click(c, interval, precise)
                if shift then
                    Keyboard.Release(Keyboard.LEFT_SHIFT)
                    shift = false
                end
            elseif c == "-" then
                Keyboard.Click(Keyboard.MINUS, interval, precise)
            elseif c == "=" then
                Keyboard.Click(Keyboard.EQUAL, interval, precise)
            elseif c == "[" then
                Keyboard.Click(Keyboard.LBRACKET, interval, precise)
            elseif c == "]" then
                Keyboard.Click(Keyboard.RBRACKET, interval, precise)
            elseif c == "\\" then
                Keyboard.Click(Keyboard.BACKSLASH, interval, precise)
            elseif c == ";" then
                Keyboard.Click(Keyboard.SEMICOLON, interval, precise)
            elseif c == "'" then
                Keyboard.Click(Keyboard.QUOTE, interval, precise)
            elseif c == "," then
                Keyboard.Click(Keyboard.COMMA, interval, precise)
            elseif c == "." then
                Keyboard.Click(Keyboard.PERIOD, interval, precise)
            elseif c == "/" then
                Keyboard.Click(Keyboard.SLASH, interval, precise)
            else
                Keyboard.Press(Keyboard.LEFT_SHIFT)
                if c == "~" then
                    Keyboard.Click(Keyboard.BACKQUOTE, interval, precise)
                elseif c == "!" then
                    Keyboard.Click(Keyboard.ONE, interval, precise)
                elseif c == "@" then
                    Keyboard.Click(Keyboard.TWO, interval, precise)
                elseif c == "#" then
                    Keyboard.Click(Keyboard.THREE, interval, precise)
                elseif c == "$" then
                    Keyboard.Click(Keyboard.FOUR, interval, precise)
                elseif c == "%" then
                    Keyboard.Click(Keyboard.FIVE, interval, precise)
                elseif c == "^" then
                    Keyboard.Click(Keyboard.SIX, interval, precise)
                elseif c == "&" then
                    Keyboard.Click(Keyboard.SEVEN, interval, precise)
                elseif c == "*" then
                    Keyboard.Click(Keyboard.EIGHT, interval, precise)
                elseif c == "(" then
                    Keyboard.Click(Keyboard.NINE, interval, precise)
                elseif c == ")" then
                    Keyboard.Click(Keyboard.ZERO, interval, precise)
                elseif c == "_" then
                    Keyboard.Click(Keyboard.MINUS, interval, precise)
                elseif c == "+" then
                    Keyboard.Click(Keyboard.EQUAL, interval, precise)
                elseif c == "{" then
                    Keyboard.Click(Keyboard.LBRACKET, interval, precise)
                elseif c == "}" then
                    Keyboard.Click(Keyboard.RBRACKET, interval, precise)
                elseif c == "|" then
                    Keyboard.Click(Keyboard.BACKSLASH, interval, precise)
                elseif c == ":" then
                    Keyboard.Click(Keyboard.SEMICOLON, interval, precise)
                elseif c == '"' then
                    Keyboard.Click(Keyboard.QUOTE, interval, precise)
                elseif c == "<" then
                    Keyboard.Click(Keyboard.COMMA, interval, precise)
                elseif c == ">" then
                    Keyboard.Click(Keyboard.PERIOD, interval, precise)
                elseif c == "?" then
                    Keyboard.Click(Keyboard.SLASH, interval, precise)
                end
                Keyboard.Release(Keyboard.LEFT_SHIFT)
            end
        end
    end
    -- TODO: Await this call when Runtime.sleep returns a Promise.
    Runtime.sleep(delay, precise)
end

---判断修饰键（SHIFT、CTRL、ALT）是否按下。
---@param key Key 可取 `Keyboard.SHIFT`、`Keyboard.CTRL`、`Keyboard.ALT`、`Keyboard.LEFT_SHIFT`、`Keyboard.RIGHT_SHIFT`、`Keyboard.LEFT_CTRL`、`Keyboard.RIGHT_CTRL`、`Keyboard.LEFT_ALT`、`Keyboard.RIGHT_ALT`
---@return boolean # 指定修饰键是否按下
function Keyboard.IsModifierPressed(key)
    return IsModifierPressed(key)
end

---判断锁定键（CAPSLOCK、SCROLLLOCK、NUMLOCK）是否处于开启状态。
---@param key Key 按键名称，可取 `Keyboard.CAPS_LOCK`、`Keyboard.SCROLL_LOCK`、`Keyboard.NUM_LOCK`
---@return boolean
function Keyboard.IsKeyLockOn(key)
    return IsKeyLockOn(key)
end

L.BeforeUnload(function ()
    Keyboard.Freeze()
    Keyboard.reset()
end)

return Keyboard