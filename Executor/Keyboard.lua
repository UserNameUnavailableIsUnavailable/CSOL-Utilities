if not Keyboard_lua then
    Keyboard_lua = true

    Include("Emulator.lua")
    Include("Context.lua")
    Include("Runtime.lua")
    Include("Error.lua")
    Include("Delay.lua")
    Include("Constants.lua")
    Include("Version.lua")
    Version:set("Keyboard", "1.5.2")
    ---@class Keyboard
    Keyboard = {
        -- 功能区
        ESCAPE = Constants.keyboard_names.ESCAPE,
        F1 = Constants.keyboard_names.F1,
        F2 = Constants.keyboard_names.F2,
        F3 = Constants.keyboard_names.F3,
        F4 = Constants.keyboard_names.F4,
        F5 = Constants.keyboard_names.F5,
        F6 = Constants.keyboard_names.F6,
        F7 = Constants.keyboard_names.F7,
        F8 = Constants.keyboard_names.F8,
        F9 = Constants.keyboard_names.F9,
        F10 = Constants.keyboard_names.F10,
        F11 = Constants.keyboard_names.F11,
        F12 = Constants.keyboard_names.F12,
        F13 = Constants.keyboard_names.F13,
        F14 = Constants.keyboard_names.F14,
        F15 = Constants.keyboard_names.F15,
        F16 = Constants.keyboard_names.F16,
        F17 = Constants.keyboard_names.F17,
        F18 = Constants.keyboard_names.F18,
        F19 = Constants.keyboard_names.F19,
        F20 = Constants.keyboard_names.F20,
        F21 = Constants.keyboard_names.F21,
        F22 = Constants.keyboard_names.F22,
        F23 = Constants.keyboard_names.F23,
        F24 = Constants.keyboard_names.F24,
        -- 主区
        BACKQUOTE = Constants.keyboard_names.BACKQUOTE,
        ONE = Constants.keyboard_names.ONE,
        TWO = Constants.keyboard_names.TWO,
        THREE = Constants.keyboard_names.THREE,
        FOUR = Constants.keyboard_names.FOUR,
        FIVE = Constants.keyboard_names.FIVE,
        SIX = Constants.keyboard_names.SIX,
        SEVEN = Constants.keyboard_names.SEVEN,
        EIGHT = Constants.keyboard_names.EIGHT,
        NINE = Constants.keyboard_names.NINE,
        ZERO = Constants.keyboard_names.ZERO,
        MINUS = Constants.keyboard_names.MINUS,
        EQUAL = Constants.keyboard_names.EQUAL,
        BACKSPACE = Constants.keyboard_names.BACKSPACE,
        TAB = Constants.keyboard_names.TAB,
        Q = Constants.keyboard_names.Q,
        W = Constants.keyboard_names.W,
        E = Constants.keyboard_names.E,
        R = Constants.keyboard_names.R,
        T = Constants.keyboard_names.T,
        Y = Constants.keyboard_names.Y,
        U = Constants.keyboard_names.U,
        I = Constants.keyboard_names.I,
        O = Constants.keyboard_names.O,
        P = Constants.keyboard_names.P,
        LBRACKET = Constants.keyboard_names.LBRACKET,
        RBRACKET = Constants.keyboard_names.RBRACKET,
        BACKSLASH = Constants.keyboard_names.BACKSLASH,
        CAPS_LOCK = Constants.keyboard_names.CAPS_LOCK,
        A = Constants.keyboard_names.A,
        S = Constants.keyboard_names.S,
        D = Constants.keyboard_names.D,
        F = Constants.keyboard_names.F,
        G = Constants.keyboard_names.G,
        H = Constants.keyboard_names.H,
        J = Constants.keyboard_names.J,
        K = Constants.keyboard_names.K,
        L = Constants.keyboard_names.L,
        SEMICOLON = Constants.keyboard_names.SEMICOLON,
        QUOTE = Constants.keyboard_names.QUOTE,
        ENTER = Constants.keyboard_names.ENTER,
        LEFT_SHIFT = Constants.keyboard_names.LEFT_SHIFT,
        NON_US_SLASH = Constants.keyboard_names.NON_US_SLASH,
        Z = Constants.keyboard_names.Z,
        X = Constants.keyboard_names.X,
        C = Constants.keyboard_names.C,
        V = Constants.keyboard_names.V,
        B = Constants.keyboard_names.B,
        N = Constants.keyboard_names.N,
        M = Constants.keyboard_names.M,
        COMMA = Constants.keyboard_names.COMMA,
        PERIOD = Constants.keyboard_names.PERIOD,
        SLASH = Constants.keyboard_names.SLASH,
        RIGHT_SHIFT = Constants.keyboard_names.RIGHT_SHIFT,
        LEFT_CTRL = Constants.keyboard_names.LEFT_CTRL,
        LEFT_ALT = Constants.keyboard_names.LEFT_ALT,
        LEFT_WIN = Constants.keyboard_names.LEFT_WIN,
        SPACE = Constants.keyboard_names.SPACE,
        RIGHT_ALT = Constants.keyboard_names.RIGHT_ALT,
        RIGHT_WIN = Constants.keyboard_names.RIGHT_WIN,
        APPS = Constants.keyboard_names.APPS,
        RIGHT_CTRL = Constants.keyboard_names.RIGHT_CTRL,
        -- 数字区（小键盘）
        NUM_LOCK = Constants.keyboard_names.NUM_LOCK,
        NUM_ZERO = Constants.keyboard_names.NUM_ZERO,
        NUM_ONE = Constants.keyboard_names.NUM_ONE,
        NUM_TWO = Constants.keyboard_names.NUM_TWO,
        NUM_THREE = Constants.keyboard_names.NUM_THREE,
        NUM_FOUR = Constants.keyboard_names.NUM_FOUR,
        NUM_FIVE = Constants.keyboard_names.NUM_FIVE,
        NUM_SIX = Constants.keyboard_names.NUM_SIX,
        NUM_SEVEN = Constants.keyboard_names.NUM_SEVEN,
        NUM_EIGHT = Constants.keyboard_names.NUM_EIGHT,
        NUM_NINE = Constants.keyboard_names.NUM_NINE,
        NUM_DECIMAL = Constants.keyboard_names.NUM_DECIMAL,
        NUM_ENTER = Constants.keyboard_names.NUM_ENTER,
        NUM_PLUS = Constants.keyboard_names.NUM_PLUS,
        NUM_MINUS = Constants.keyboard_names.NUM_MINUS,
        NUM_MULTIPLY = Constants.keyboard_names.NUM_MULTIPLY,
        NUM_DIVIDE = Constants.keyboard_names.NUM_DIVIDE,
        -- 控制区
        PRINT_SCREEN = Constants.keyboard_names.PRINT_SCREEN,
        SCROLL_LOCK = Constants.keyboard_names.SCROLL_LOCK,
        PAUSE_BREAK = Constants.keyboard_names.PAUSE_BREAK,
        INSERT = Constants.keyboard_names.INSERT,
        DELETE = Constants.keyboard_names.DELETE,
        HOME = Constants.keyboard_names.HOME,
        END = Constants.keyboard_names.END,
        PAGE_UP = Constants.keyboard_names.PAGE_UP,
        PAGE_DOWN = Constants.keyboard_names.PAGE_DOWN,
        UP = Constants.keyboard_names.UP,
        DOWN = Constants.keyboard_names.DOWN,
        LEFT = Constants.keyboard_names.LEFT,
        RIGHT = Constants.keyboard_names.RIGHT,

        -- 注意：这三个不区分左右的修饰键只能用于 `is_modifier_pressed` 中
        CTRL = "ctrl",
        SHIFT = "shift",
        ALT = "alt",
    }

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

    function Keyboard:is_frozen()
        return self.frozen
    end

    ---@param key string|string[] 按键名称，如 `Keyboard.F1`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay integer|nil 按下按键后的延迟时间。可以直接使用预定义于Delay表中的字段，如 `Delay.NORMAL`。
    ---@param precise boolean|nil 是否精确定时
    ---按下按键。
    function Keyboard:press(key, delay, precise)
        if not self:is_frozen() then
            if type(key) == "string" and self:is_key_valid(key) then
                self.unreleased[key] = true
                PressKey(key)
            end
            if type(key) == "table" then
                local keys = {}
                for _, v in ipairs(key) do
                    if self:is_key_valid(v) then
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
    ---@param key string|table 按键名称，如 `Keyboard.ESCAPE`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay integer|nil 按下按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
    ---@param precise boolean|nil 是否精确定时
    function Keyboard:release(key, delay, precise)
        if not self:is_frozen() then
            if type(key) == "string" and self:is_key_valid(key) then
                self.unreleased[key] = nil
                ReleaseKey(key)
            end
            if type(key) == "table" then
                local keys = {}
                for _, v in ipairs(key) do
                    if self:is_key_valid(v) then
                        keys[#keys + 1] = v
                        self.unreleased[v] = nil
                    end
                end
                ReleaseKey(table.unpack(keys))
            end
        end
        Runtime:sleep(delay, precise)
    end

    ---按下，而后弹起按键（视为一次点击）。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param key string|table 按键名称，如 `Keyboard.ESCAPE`
    ---@param delay integer|nil 点击按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
    ---@param precise boolean|nil 是否精确定时
    function Keyboard:click(key, delay, precise)
        if not self:is_frozen() then
            if type(key) == "string" and self:is_key_valid(key) then
                PressAndReleaseKey(key)
            end
            if type(key) == "table" then
                local keys = {}
                for _, v in ipairs(key) do
                    if self:is_key_valid(v) then
                        keys[#keys + 1] = v
                    end
                end
                PressAndReleaseKey(table.unpack(keys))
            end
        end
        Runtime:sleep(delay, precise)
    end

    ---释放所有按键。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    function Keyboard:reset()
        for key, _ in pairs(self.unreleased) do
            ReleaseKey(key)
            Runtime:sleep(Delay.MINI)
            self.unreleased[key] = nil
        end
    end

    ---点击某个按键若干次。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param key string 按键名称，如 `Keyboard.ESCAPE`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param times integer|nil 重复次数。
    ---@param interval integer|nil 间隔
    ---@param delay integer|nil 操作完成后的延迟时间
    ---@param precise boolean|nil 是否精确定时
    function Keyboard:click_several_times(key, times, interval, delay, precise)
        interval = interval or Delay.SHORT
        delay = delay or Delay.SHORT
        times = times or 0
        if not self:is_frozen() and math.type(times) == "integer" and Keyboard:is_key_valid(key) then
            for i = 1, times do
                PressAndReleaseKey(key)
                if i ~= times then
                    Runtime:sleep(interval, precise)
                end
            end
        end
        Runtime:sleep(delay, precise)
    end

    ---检查给定的按键名是否有效。
    ---@param key string|integer 按键名或按键扫描码
    function Keyboard:is_key_valid(key)
        if type(key) == "number" then
            for _, v in pairs(Constants.keyboard_scancodes) do
                if v == key then
                    return true
                end
            end
        end
        if type(key) == "string" then
            for k, v in pairs(Constants.keyboard_names) do
                if v == key then
                    return true
                end
            end
        end
        return false
    end

    ---通过键盘输入由字母和数字构成的字符串序列
    ---@param s string ASCII 字符
    ---@param interval integer|nil
    ---@param delay integer|nil
    ---@param precise boolean|nil
    function Keyboard:puts(s, interval, delay, precise)
        interval = interval or Delay.SHORT
        delay = delay or Delay.SHORT
        if not self:is_frozen() then
            local shift = false -- shift 是否按下
            for i = 1, #s do
                local c = s:sub(i, i)
                if string.byte("0") <= string.byte(c) and string.byte(c) <= string.byte("9") then
                    Keyboard:click(c, interval, precise)
                elseif string.byte("A") <= string.byte(c) and string.byte(c) <= string.byte("Z") then
                    if
                        not IsKeyLockOn(Keyboard.CAPS_LOCK) -- 非大写锁定状态
                    then
                        Keyboard:press(Keyboard.LEFT_SHIFT)
                        shift = true
                    end
                    Keyboard:click(c:lower(), interval, precise)
                    if shift then
                        Keyboard:release(Keyboard.LEFT_SHIFT)
                        shift = false
                    end
                elseif string.byte("a") <= string.byte(c) and string.byte(c) <= string.byte("z") then
                    if
                        IsKeyLockOn(Keyboard.CAPS_LOCK) -- 大写锁定状态
                    then
                        Keyboard:press(Keyboard.LEFT_SHIFT)
                        shift = true
                    end
                    Keyboard:click(c, interval, precise)
                    if shift then
                        Keyboard:release(Keyboard.LEFT_SHIFT)
                        shift = false
                    end
                elseif c == "-" then
                    Keyboard:click(Keyboard.MINUS, interval, precise)
                elseif c == "=" then
                    Keyboard:click(Keyboard.EQUAL, interval, precise)
                elseif c == "[" then
                    Keyboard:click(Keyboard.LBRACKET, interval, precise)
                elseif c == "]" then
                    Keyboard:click(Keyboard.RBRACKET, interval, precise)
                elseif c == "\\" then
                    Keyboard:click(Keyboard.BACKSLASH, interval, precise)
                elseif c == ";" then
                    Keyboard:click(Keyboard.SEMICOLON, interval, precise)
                elseif c == "'" then
                    Keyboard:click(Keyboard.QUOTE, interval, precise)
                elseif c == "," then
                    Keyboard:click(Keyboard.COMMA, interval, precise)
                elseif c == "." then
                    Keyboard:click(Keyboard.PERIOD, interval, precise)
                elseif c == "/" then
                    Keyboard:click(Keyboard.SLASH, interval, precise)
                else
                    Keyboard:press(Keyboard.LEFT_SHIFT)
                    if c == "~" then
                        Keyboard:click(Keyboard.BACKQUOTE, interval, precise)
                    elseif c == "!" then
                        Keyboard:click(Keyboard.ONE, interval, precise)
                    elseif c == "@" then
                        Keyboard:click(Keyboard.TWO, interval, precise)
                    elseif c == "#" then
                        Keyboard:click(Keyboard.THREE, interval, precise)
                    elseif c == "$" then
                        Keyboard:click(Keyboard.FOUR, interval, precise)
                    elseif c == "%" then
                        Keyboard:click(Keyboard.FIVE, interval, precise)
                    elseif c == "^" then
                        Keyboard:click(Keyboard.SIX, interval, precise)
                    elseif c == "&" then
                        Keyboard:click(Keyboard.SEVEN, interval, precise)
                    elseif c == "*" then
                        Keyboard:click(Keyboard.EIGHT, interval, precise)
                    elseif c == "(" then
                        Keyboard:click(Keyboard.NINE, interval, precise)
                    elseif c == ")" then
                        Keyboard:click(Keyboard.ZERO, interval, precise)
                    elseif c == "_" then
                        Keyboard:click(Keyboard.MINUS, interval, precise)
                    elseif c == "+" then
                        Keyboard:click(Keyboard.EQUAL, interval, precise)
                    elseif c == "{" then
                        Keyboard:click(Keyboard.LBRACKET, interval, precise)
                    elseif c == "}" then
                        Keyboard:click(Keyboard.RBRACKET, interval, precise)
                    elseif c == "|" then
                        Keyboard:click(Keyboard.BACKSLASH, interval, precise)
                    elseif c == ":" then
                        Keyboard:click(Keyboard.SEMICOLON, interval, precise)
                    elseif c == '"' then
                        Keyboard:click(Keyboard.QUOTE, interval, precise)
                    elseif c == "<" then
                        Keyboard:click(Keyboard.COMMA, interval, precise)
                    elseif c == ">" then
                        Keyboard:click(Keyboard.PERIOD, interval, precise)
                    elseif c == "?" then
                        Keyboard:click(Keyboard.SLASH, interval, precise)
                    end
                    Keyboard:release(Keyboard.LEFT_SHIFT)
                end
            end
        end
        Runtime:sleep(delay, precise)
    end

    ---判断修饰键（SHIFT、CTRL、ALT）是否按下。
    ---@param key string 按键名称，如 `Keyboard.LALT`。
    ---@return boolean # 指定修饰键是否按下
    function Keyboard:is_modifier_pressed(key)
        return IsModifierPressed(key)
    end

    ---判断锁定键（CAPSLOCK、SCROLLLOCK、NUMLOCK）是否处于开启状态。
    ---@param key string 按键名称
    ---@return boolean
    function Keyboard:is_key_lock_on(key)
        return IsKeyLockOn(key)
    end

    Error:register_fatal_disposal(function()
        Keyboard:reset()
    end)
end -- Keyboard_lua
