if (not Keyboard_lua)
then
    Keyboard_lua = true
    Include("Context.lua")
    Include("Runtime.lua")
    Include("Error.lua")
    Include("Delay.lua")
    ---@class Keyboard
    ---@enum
    Keyboard = {}

    local valid_keynames = {
        -- 功能区
        ESCAPE = "escape",
        F1 = "f1", F2 = "f2", F3 = "f3", F4 = "f4", F5 = "f5", F6 = "f6",
        F7 = "f7", F8 = "f8", F9 = "f9", F10 = "f10", F11 = "f11", F12 = "f12",
        F13 = "f13", F14 = "f14", F15 = "f15", F16 = "f16", F17 = "f17", F18 = "f18",
        F19 = "f19", F20 = "f20", F21 = "f21", F22 = "f22", F23 = "f23", F24 = "f24",
        -- 主区
        BACKQUOTE = "tilde", ONE = "1", TWO = "2", THREE = "3", FOUR = "4", FIVE = "5", SIX = "6", SEVEN = "7",
        EIGHT = "8", NINE = "9", ZERO = "0", MINUS = "minus", EQUAL = "equal", BACKSPACE = "backspace",

        TAB = "tab", Q = "q", W = "w", E = "e", R = "r", T = "t", Y = "y", U = "u", I = "i", O = "o",
        P = "p", LBRACKET = "lbracket", RBRACKET = "rbracket", BACKSLASH = "backslash",

        CAPS_LOCK = "capslock", A = "a", S = "s", D = "d", F = "f", G = "g", H = "h", J = "j", K = "k", L = "l",
        SEMICOLON = "semicolon", QUOTE = "quote", ENTER = "enter",

        LSHIFT = "lshift", NON_US_BACKSLASH = "non_us_slash", Z = "z", X = "x", C = "c", V = "v", B = "b", N = "n", M = "m",
        COMMA = "comma", PERIOD = "period", SLASH = "slash", RSHIFT = "rshift",
        
        LCTRL = "lctrl", LWIN = "lgui", LALT = "lalt", SPACE = "spacebar",
        RALT = "ralt", RWIN = "rgui", APPS = "appkey", RCTRL = "rctrl",
        -- 控制区
        PRINT_SCREEN = "printscreen", SCROLL_LOCK = "scrolllock", PAUSE_BREAK = "pause",
        INSERT = "insert", HOME = "home", PAGE_UP = "pageup",
        DELETE = "delete", END = "end", PAGE_DOWN = "pagedown",

        UP = "up", LEFT = "left", DOWN = "down", RIGHT = "right",

        -- 数字区
        NUM_LOCK = "numlock", NUM_DIVIDE = "numslash", NUM_MINUS = "numminus", NUM_MULTIPLY = "numstar",
        NUM_SEVEN = "num7", NUM_EIGHT = "num8", NUM_NINE = "num9", NUM_PLUS = "numplus",
        NUM_FOUR = "num4", NUM_FIVE = "num5", NUM_SIX = "num6",
        NUM_ONE = "num1", NUM_TWO = "num2", NUM_THREE = "num3",
        NUM_ZERO = "num0", NUM_PERIOD = "numperiod", NUM_ENTER = "numenter",
    }

    for k, v in pairs(valid_keynames)
    do
        Keyboard[k] = v
    end

    -- 注意：这三个不区分左右的修饰键只能用于 `is_modifier_pressed` 中
    Keyboard.CTRL = "ctrl"
    Keyboard.SHIFT = "shift"
    Keyboard.ALT = "alt"

    ---通过 `press` 按下但尚未通过 `release` 释放的按键。
    ---@type { [string]: boolean }
    Keyboard.unreleased = {}

    Keyboard.frozen = false

    function Keyboard:freeze()
        self.frozen = true
    end

    function Keyboard:unfreeze()
        self.frozen = false
    end

    ---@param key string | string[] 按键名称，如 `Keyboard.F1`。当 `Keyboard.frozen` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay integer | nil 按下按键后的延迟时间。可以直接使用预定义于Delay表中的字段，如 `Delay.NORMAL`。
    ---@param precise boolean | nil 是否精确定时
    ---按下按键。
    function Keyboard:press(key, delay, precise)
        if (not Keyboard.frozen)
        then
            if (type(key) == "string" and self:is_key_name_valid(key))
            then
                self.unreleased[key] = true
                PressKey(key)
            end
            if (type(key) == "table")
            then
                local keys = {}
                for _, v in ipairs(key)
                do
                    if (self:is_key_name_valid(v))
                    then
                        keys[#keys + 1] = v
                        self.unreleased[v] = true
                    end
                end
                PressKey(table.unpack(keys))
            end
        end
        Runtime:sleep(delay, precise)
    end

    ---弹起按键。
    ---@param key string | table 按键名称，如 `Keyboard.ESCAPE`。当 `Keyboard.frozen` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay integer | nil 按下按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
    ---@param precise boolean | nil 是否精确定时
    function Keyboard:release(key, delay, precise)
        if (not Keyboard.frozen)
        then
            if (type(key) == "string" and self:is_key_name_valid(key))
            then
                self.unreleased[key] = nil
                ReleaseKey(key)
            end
            if (type(key) == "table")
            then
                local keys = {}
                for _, v in ipairs(key)
                do
                    if (self:is_key_name_valid(v))
                    then
                        keys[#keys + 1] = v
                        self.unreleased[v] = nil
                    end
                end
                ReleaseKey(table.unpack(keys))
            end
        end
        Runtime:sleep(delay, precise)
    end

    ---按下，而后弹起按键（视为一次点击）。当 `Keyboard.frozen` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param key string | table 按键名称，如 `Keyboard.ESCAPE`
    ---@param delay integer | nil 点击按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
    ---@param precise boolean | nil 是否精确定时
    function Keyboard:click(key, delay, precise)
        if (not Keyboard.frozen)
        then
            if (type(key) == "string" and self:is_key_name_valid(key))
            then
                PressAndReleaseKey(key)
            end
            if (type(key) == "table")
            then
                local keys = {}
                for _, v in ipairs(key)
                do
                    if (self:is_key_name_valid(v))
                    then
                        keys[#keys + 1] = v
                    end
                end
                PressAndReleaseKey(table.unpack(keys))
            end
        end
        Runtime:sleep(delay, precise)
    end

    ---释放所有按键。当 `Keyboard.frozen` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay integer | nil 每释放一个按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
    ---@param precise boolean | nil 是否精确定时
    function Keyboard:reset(delay, precise)
        for key, _ in pairs(self.unreleased)
        do
            Keyboard:release(key, delay or 0, precise)
        end
    end

    ---点击某个按键若干次。当 `Keyboard.frozen` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param key string 按键名称，如 `Keyboard.ESCAPE`。当 `Keyboard.frozen` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param times integer | nil 重复次数。
    ---@param delay integer | nil 每释放一个按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
    ---@param precise boolean | nil 是否精确定时
    function Keyboard:click_several_times(key, times, delay, precise)
        times = times or 0
        delay = delay or Delay.SHORT
        for i = 1, times
        do
            Keyboard:click(key, delay, precise)
        end
        Runtime:sleep(delay, precise)
    end

    ---检查给定的按键名是否有效。
    ---@param key_name string 按键名
    function Keyboard:is_key_name_valid(key_name)
        for _, v in pairs(valid_keynames)
        do
            if (v == key_name)
            then
                return true
            end
        end
        return false
    end

    ---通过键盘输入由字母和数字构成的字符串序列
    ---@param s string ASCII 字符
    function Keyboard:puts(s)
        local shift = false -- shift 是否按下
        for i = 1, #s
        do
            local c = s:sub(i, i)
            if (string.byte("0") <= string.byte(c) and string.byte(c) <= string.byte("9"))
            then
                Keyboard:click(c, 100)
            elseif (string.byte("A") <= string.byte(c) and string.byte(c) <= string.byte("Z"))
            then
                if (not IsKeyLockOn(Keyboard.CAPS_LOCK)) -- 非大写锁定状态
                then
                    Keyboard:press(Keyboard.LSHIFT)
                    shift = true
                end
                Keyboard:click(c, 100)
                if (shift)
                then
                    Keyboard:release(Keyboard.LSHIFT)
                    shift = false
                end
            elseif (string.byte("a") <= string.byte(c) and string.byte(c) <= string.byte("z"))
            then
                if (IsKeyLockOn(Keyboard.CAPS_LOCK)) -- 大写锁定状态
                then
                    Keyboard:press(Keyboard.LSHIFT)
                    shift = true
                end
                Keyboard:click(c, 100)
                if (shift)
                then
                    Keyboard:release(Keyboard.LSHIFT)
                    shift = false
                end
            elseif (c == '-') then Keyboard:click(Keyboard.MINUS)
            elseif (c == '=') then Keyboard:click(Keyboard.EQUAL)
            elseif (c == '[') then Keyboard:click(Keyboard.LBRACKET)
            elseif (c == ']') then Keyboard:click(Keyboard.RBRACKET)
            elseif (c == '\\') then Keyboard:click(Keyboard.BACKSLASH)
            elseif (c == ';') then Keyboard:click(Keyboard.SEMICOLON)
            elseif (c == '\'') then Keyboard:click(Keyboard.QUOTE)
            elseif (c == ',') then Keyboard:click(Keyboard.COMMA)
            elseif (c == '.') then Keyboard:click(Keyboard.PERIOD)
            elseif (c == '/') then Keyboard:click(Keyboard.SLASH)
            else
                Keyboard:press(Keyboard.LSHIFT)
                if (c == '~') then Keyboard:click(Keyboard.BACKQUOTE)
                elseif (c == '!') then Keyboard:click(Keyboard.ONE)
                elseif (c == '@') then Keyboard:click(Keyboard.TWO)
                elseif (c == '#') then Keyboard:click(Keyboard.THREE)
                elseif (c == '$') then Keyboard:click(Keyboard.FOUR)
                elseif (c == '%') then Keyboard:click(Keyboard.FIVE)
                elseif (c == '^') then Keyboard:click(Keyboard.SIX)
                elseif (c == '&') then Keyboard:click(Keyboard.SEVEN)
                elseif (c == '*') then Keyboard:click(Keyboard.EIGHT)
                elseif (c == '(') then Keyboard:click(Keyboard.NINE)
                elseif (c == ')') then Keyboard:click(Keyboard.ZERO)
                elseif (c == '_') then Keyboard:click(Keyboard.MINUS)
                elseif (c == '+') then Keyboard:click(Keyboard.EQUAL)
                elseif (c == '{') then Keyboard:click(Keyboard.LBRACKET)
                elseif (c == '}') then Keyboard:click(Keyboard.RBRACKET)
                elseif (c == '|') then Keyboard:click(Keyboard.BACKSLASH)
                elseif (c == ':') then Keyboard:click(Keyboard.SEMICOLON)
                elseif (c == '"') then Keyboard:click(Keyboard.QUOTE)
                elseif (c == '<') then Keyboard:click(Keyboard.COMMA)
                elseif (c == '>') then Keyboard:click(Keyboard.PERIOD)
                elseif (c == '?') then Keyboard:click(Keyboard.SLASH)
                end
                Keyboard:release(Keyboard.LSHIFT)
            end
        end
    end

    ---判断修饰键（SHIFT、CTRL、ALT）是否按下。
    ---@param key string 按键名称，如 `Keyboard.LALT`。 
    ---@return boolean # 指定修饰键是否按下
    function Keyboard:is_modifier_pressed(key) return IsModifierPressed(key) end

    ---判断锁定键（CAPSLOCK、SCROLLLOCK、NUMLOCK）是否处于开启状态。
    ---@param key string 按键名称
    ---@return boolean
    function Keyboard:is_key_lock_on(key) return IsKeyLockOn(key) end

    Error:register_fatal_disposal(
        function ()
            Keyboard:reset(0)
        end
    )

end -- Keyboard_lua