if not __KEYBOARD_LUA__ then
    __KEYBOARD_LUA__ = true
    local __version__ = "1.5.4"

    Include("Constants.lua")
    Include("Emulator.lua")
    Include("Context.lua")
    Include("Runtime.lua")
    Include("Exception.lua")
    Include("Delay.lua")
    Include("Constants.lua")
    Include("Version.lua")
    Version:set("Keyboard", __version__)

    ---@class Keyboard
    ---@field ESCAPE KEYBOARD_NAME
    ---@field F1 KEYBOARD_NAME
    ---@field F2 KEYBOARD_NAME
    ---@field F3 KEYBOARD_NAME
    ---@field F4 KEYBOARD_NAME
    ---@field F5 KEYBOARD_NAME
    ---@field F6 KEYBOARD_NAME
    ---@field F7 KEYBOARD_NAME
    ---@field F8 KEYBOARD_NAME
    ---@field F9 KEYBOARD_NAME
    ---@field F10 KEYBOARD_NAME
    ---@field F11 KEYBOARD_NAME
    ---@field F12 KEYBOARD_NAME
    ---@field F13 KEYBOARD_NAME
    ---@field F14 KEYBOARD_NAME
    ---@field F15 KEYBOARD_NAME
    ---@field F16 KEYBOARD_NAME
    ---@field F17 KEYBOARD_NAME
    ---@field F18 KEYBOARD_NAME
    ---@field F19 KEYBOARD_NAME
    ---@field F20 KEYBOARD_NAME
    ---@field F21 KEYBOARD_NAME
    ---@field F22 KEYBOARD_NAME
    ---@field F23 KEYBOARD_NAME
    ---@field F24 KEYBOARD_NAME
    ---@field BACKQUOTE KEYBOARD_NAME
    ---@field ONE KEYBOARD_NAME
    ---@field TWO KEYBOARD_NAME
    ---@field THREE KEYBOARD_NAME
    ---@field FOUR KEYBOARD_NAME
    ---@field FIVE KEYBOARD_NAME
    ---@field SIX KEYBOARD_NAME
    ---@field SEVEN KEYBOARD_NAME
    ---@field EIGHT KEYBOARD_NAME
    ---@field NINE KEYBOARD_NAME
    ---@field ZERO KEYBOARD_NAME
    ---@field MINUS KEYBOARD_NAME
    ---@field EQUAL KEYBOARD_NAME
    ---@field BACKSPACE KEYBOARD_NAME
    ---@field TAB KEYBOARD_NAME
    ---@field Q KEYBOARD_NAME
    ---@field W KEYBOARD_NAME
    ---@field E KEYBOARD_NAME
    ---@field R KEYBOARD_NAME
    ---@field T KEYBOARD_NAME
    ---@field Y KEYBOARD_NAME
    ---@field U KEYBOARD_NAME
    ---@field I KEYBOARD_NAME
    ---@field O KEYBOARD_NAME
    ---@field P KEYBOARD_NAME
    ---@field LBRACKET KEYBOARD_NAME
    ---@field RBRACKET KEYBOARD_NAME
    ---@field BACKSLASH KEYBOARD_NAME
    ---@field CAPS_LOCK KEYBOARD_NAME
    ---@field A KEYBOARD_NAME
    ---@field S KEYBOARD_NAME
    ---@field D KEYBOARD_NAME
    ---@field F KEYBOARD_NAME
    ---@field G KEYBOARD_NAME
    ---@field H KEYBOARD_NAME
    ---@field J KEYBOARD_NAME
    ---@field K KEYBOARD_NAME
    ---@field L KEYBOARD_NAME
    ---@field SEMICOLON KEYBOARD_NAME
    ---@field QUOTE KEYBOARD_NAME
    ---@field ENTER KEYBOARD_NAME
    ---@field LEFT_SHIFT KEYBOARD_NAME
    ---@field NON_US_SLASH KEYBOARD_NAME
    ---@field Z KEYBOARD_NAME
    ---@field X KEYBOARD_NAME
    ---@field C KEYBOARD_NAME
    ---@field V KEYBOARD_NAME
    ---@field B KEYBOARD_NAME
    ---@field N KEYBOARD_NAME
    ---@field M KEYBOARD_NAME
    ---@field COMMA KEYBOARD_NAME
    ---@field PERIOD KEYBOARD_NAME
    ---@field SLASH KEYBOARD_NAME
    ---@field RIGHT_SHIFT KEYBOARD_NAME
    ---@field LEFT_CTRL KEYBOARD_NAME
    ---@field LEFT_ALT KEYBOARD_NAME
    ---@field LEFT_WIN KEYBOARD_NAME
    ---@field SPACE KEYBOARD_NAME
    ---@field RIGHT_ALT KEYBOARD_NAME
    ---@field RIGHT_WIN KEYBOARD_NAME
    ---@field APPS KEYBOARD_NAME
    ---@field RIGHT_CTRL KEYBOARD_NAME
    ---@field NUM_LOCK KEYBOARD_NAME
    ---@field NUM_ZERO KEYBOARD_NAME
    ---@field NUM_ONE KEYBOARD_NAME
    ---@field NUM_TWO KEYBOARD_NAME
    ---@field NUM_THREE KEYBOARD_NAME
    ---@field NUM_FOUR KEYBOARD_NAME
    ---@field NUM_FIVE KEYBOARD_NAME
    ---@field NUM_SIX KEYBOARD_NAME
    ---@field NUM_SEVEN KEYBOARD_NAME
    ---@field NUM_EIGHT KEYBOARD_NAME
    ---@field NUM_NINE KEYBOARD_NAME
    ---@field NUM_DECIMAL KEYBOARD_NAME
    ---@field NUM_ENTER KEYBOARD_NAME
    ---@field NUM_PLUS KEYBOARD_NAME
    ---@field NUM_MINUS KEYBOARD_NAME
    ---@field NUM_MULTIPLY KEYBOARD_NAME
    ---@field NUM_DIVIDE KEYBOARD_NAME
    ---@field PRINT_SCREEN KEYBOARD_NAME
    ---@field SCROLL_LOCK KEYBOARD_NAME
    ---@field PAUSE_BREAK KEYBOARD_NAME
    ---@field INSERT KEYBOARD_NAME
    ---@field DELETE KEYBOARD_NAME
    ---@field HOME KEYBOARD_NAME
    ---@field END KEYBOARD_NAME
    ---@field PAGE_UP KEYBOARD_NAME
    ---@field PAGE_DOWN KEYBOARD_NAME
    ---@field UP KEYBOARD_NAME
    ---@field DOWN KEYBOARD_NAME
    ---@field LEFT KEYBOARD_NAME
    ---@field RIGHT KEYBOARD_NAME
    ---@field CTRL KEYBOARD_NAME
    ---@field SHIFT KEYBOARD_NAME
    ---@field ALT KEYBOARD_NAME
    Keyboard = {
        -- 功能区
        ESCAPE = Constants.KEYBOARD_NAME.ESCAPE,
        F1 = Constants.KEYBOARD_NAME.F1,
        F2 = Constants.KEYBOARD_NAME.F2,
        F3 = Constants.KEYBOARD_NAME.F3,
        F4 = Constants.KEYBOARD_NAME.F4,
        F5 = Constants.KEYBOARD_NAME.F5,
        F6 = Constants.KEYBOARD_NAME.F6,
        F7 = Constants.KEYBOARD_NAME.F7,
        F8 = Constants.KEYBOARD_NAME.F8,
        F9 = Constants.KEYBOARD_NAME.F9,
        F10 = Constants.KEYBOARD_NAME.F10,
        F11 = Constants.KEYBOARD_NAME.F11,
        F12 = Constants.KEYBOARD_NAME.F12,
        F13 = Constants.KEYBOARD_NAME.F13,
        F14 = Constants.KEYBOARD_NAME.F14,
        F15 = Constants.KEYBOARD_NAME.F15,
        F16 = Constants.KEYBOARD_NAME.F16,
        F17 = Constants.KEYBOARD_NAME.F17,
        F18 = Constants.KEYBOARD_NAME.F18,
        F19 = Constants.KEYBOARD_NAME.F19,
        F20 = Constants.KEYBOARD_NAME.F20,
        F21 = Constants.KEYBOARD_NAME.F21,
        F22 = Constants.KEYBOARD_NAME.F22,
        F23 = Constants.KEYBOARD_NAME.F23,
        F24 = Constants.KEYBOARD_NAME.F24,
        -- 主区
        BACKQUOTE = Constants.KEYBOARD_NAME.BACKQUOTE,
        ONE = Constants.KEYBOARD_NAME.ONE,
        TWO = Constants.KEYBOARD_NAME.TWO,
        THREE = Constants.KEYBOARD_NAME.THREE,
        FOUR = Constants.KEYBOARD_NAME.FOUR,
        FIVE = Constants.KEYBOARD_NAME.FIVE,
        SIX = Constants.KEYBOARD_NAME.SIX,
        SEVEN = Constants.KEYBOARD_NAME.SEVEN,
        EIGHT = Constants.KEYBOARD_NAME.EIGHT,
        NINE = Constants.KEYBOARD_NAME.NINE,
        ZERO = Constants.KEYBOARD_NAME.ZERO,
        MINUS = Constants.KEYBOARD_NAME.MINUS,
        EQUAL = Constants.KEYBOARD_NAME.EQUAL,
        BACKSPACE = Constants.KEYBOARD_NAME.BACKSPACE,
        TAB = Constants.KEYBOARD_NAME.TAB,
        Q = Constants.KEYBOARD_NAME.Q,
        W = Constants.KEYBOARD_NAME.W,
        E = Constants.KEYBOARD_NAME.E,
        R = Constants.KEYBOARD_NAME.R,
        T = Constants.KEYBOARD_NAME.T,
        Y = Constants.KEYBOARD_NAME.Y,
        U = Constants.KEYBOARD_NAME.U,
        I = Constants.KEYBOARD_NAME.I,
        O = Constants.KEYBOARD_NAME.O,
        P = Constants.KEYBOARD_NAME.P,
        LBRACKET = Constants.KEYBOARD_NAME.LEFT_BRACKET,
        RBRACKET = Constants.KEYBOARD_NAME.RIGHT_BRACKET,
        BACKSLASH = Constants.KEYBOARD_NAME.BACKSLASH,
        CAPS_LOCK = Constants.KEYBOARD_NAME.CAPS_LOCK,
        A = Constants.KEYBOARD_NAME.A,
        S = Constants.KEYBOARD_NAME.S,
        D = Constants.KEYBOARD_NAME.D,
        F = Constants.KEYBOARD_NAME.F,
        G = Constants.KEYBOARD_NAME.G,
        H = Constants.KEYBOARD_NAME.H,
        J = Constants.KEYBOARD_NAME.J,
        K = Constants.KEYBOARD_NAME.K,
        L = Constants.KEYBOARD_NAME.L,
        SEMICOLON = Constants.KEYBOARD_NAME.SEMICOLON,
        QUOTE = Constants.KEYBOARD_NAME.QUOTE,
        ENTER = Constants.KEYBOARD_NAME.ENTER,
        LEFT_SHIFT = Constants.KEYBOARD_NAME.LEFT_SHIFT,
        NON_US_SLASH = Constants.KEYBOARD_NAME.NON_US_BACKSLASH,
        Z = Constants.KEYBOARD_NAME.Z,
        X = Constants.KEYBOARD_NAME.X,
        C = Constants.KEYBOARD_NAME.C,
        V = Constants.KEYBOARD_NAME.V,
        B = Constants.KEYBOARD_NAME.B,
        N = Constants.KEYBOARD_NAME.N,
        M = Constants.KEYBOARD_NAME.M,
        COMMA = Constants.KEYBOARD_NAME.COMMA,
        PERIOD = Constants.KEYBOARD_NAME.PERIOD,
        SLASH = Constants.KEYBOARD_NAME.SLASH,
        RIGHT_SHIFT = Constants.KEYBOARD_NAME.RIGHT_SHIFT,
        LEFT_CTRL = Constants.KEYBOARD_NAME.LEFT_CTRL,
        LEFT_ALT = Constants.KEYBOARD_NAME.LEFT_ALT,
        LEFT_WIN = Constants.KEYBOARD_NAME.LEFT_WIN,
        SPACE = Constants.KEYBOARD_NAME.SPACE,
        RIGHT_ALT = Constants.KEYBOARD_NAME.RIGHT_ALT,
        RIGHT_WIN = Constants.KEYBOARD_NAME.RIGHT_WIN,
        APPS = Constants.KEYBOARD_NAME.APPS,
        RIGHT_CTRL = Constants.KEYBOARD_NAME.RIGHT_CTRL,
        -- 数字区（小键盘）
        NUM_LOCK = Constants.KEYBOARD_NAME.NUM_LOCK,
        NUM_ZERO = Constants.KEYBOARD_NAME.NUM_ZERO,
        NUM_ONE = Constants.KEYBOARD_NAME.NUM_ONE,
        NUM_TWO = Constants.KEYBOARD_NAME.NUM_TWO,
        NUM_THREE = Constants.KEYBOARD_NAME.NUM_THREE,
        NUM_FOUR = Constants.KEYBOARD_NAME.NUM_FOUR,
        NUM_FIVE = Constants.KEYBOARD_NAME.NUM_FIVE,
        NUM_SIX = Constants.KEYBOARD_NAME.NUM_SIX,
        NUM_SEVEN = Constants.KEYBOARD_NAME.NUM_SEVEN,
        NUM_EIGHT = Constants.KEYBOARD_NAME.NUM_EIGHT,
        NUM_NINE = Constants.KEYBOARD_NAME.NUM_NINE,
        NUM_DECIMAL = Constants.KEYBOARD_NAME.NUM_DECIMAL,
        NUM_ENTER = Constants.KEYBOARD_NAME.NUM_ENTER,
        NUM_PLUS = Constants.KEYBOARD_NAME.NUM_PLUS,
        NUM_MINUS = Constants.KEYBOARD_NAME.NUM_MINUS,
        NUM_MULTIPLY = Constants.KEYBOARD_NAME.NUM_MULTIPLY,
        NUM_DIVIDE = Constants.KEYBOARD_NAME.NUM_DIVIDE,
        -- 控制区
        PRINT_SCREEN = Constants.KEYBOARD_NAME.PRINT_SCREEN,
        SCROLL_LOCK = Constants.KEYBOARD_NAME.SCROLL_LOCK,
        PAUSE_BREAK = Constants.KEYBOARD_NAME.PAUSE_BREAK,
        INSERT = Constants.KEYBOARD_NAME.INSERT,
        DELETE = Constants.KEYBOARD_NAME.DELETE,
        HOME = Constants.KEYBOARD_NAME.HOME,
        END = Constants.KEYBOARD_NAME.END,
        PAGE_UP = Constants.KEYBOARD_NAME.PAGE_UP,
        PAGE_DOWN = Constants.KEYBOARD_NAME.PAGE_DOWN,
        UP = Constants.KEYBOARD_NAME.UP,
        DOWN = Constants.KEYBOARD_NAME.DOWN,
        LEFT = Constants.KEYBOARD_NAME.LEFT,
        RIGHT = Constants.KEYBOARD_NAME.RIGHT,

        -- 注意：这三个不区分左右的修饰键只能用于 `is_modifier_pressed` 中
        CTRL = Constants.KEYBOARD_NAME.CTRL,
        SHIFT = Constants.KEYBOARD_NAME.SHIFT,
        ALT = Constants.KEYBOARD_NAME.ALT,
    }

    ---通过 `press` 按下但尚未通过 `release` 释放的按键。
    ---@type { [KEYBOARD_KEY]: boolean }
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

    ---@param key KEYBOARD_KEY|KEYBOARD_KEY[] 按键名称，如 `Keyboard.F1`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay? integer 按下按键后的延迟时间。可以直接使用预定义于Delay表中的字段，如 `Delay.NORMAL`。
    ---@param precise? boolean|nil 是否精确定时
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
    ---@param key KEYBOARD_KEY|KEYBOARD_KEY[]able 按键名称，如 `Keyboard.ESCAPE`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay? integer 按下按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
    ---@param precise? boolean 是否精确定时
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
    ---@param key KEYBOARD_KEY|KEYBOARD_KEY[] 按键名称，如 `Keyboard.ESCAPE`
    ---@param delay? integer 点击按键之后的延迟时间，单位为毫秒。可以直接使用预定义于 `Delay` 表中的字段，如 `Delay.NORMAL`。
    ---@param precise? boolean 是否精确定时
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
    ---@param key KEYBOARD_KEY 按键名称，如 `Keyboard.ESCAPE`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param times? integer 重复次数。
    ---@param interval? integer 间隔
    ---@param delay? integer 操作完成后的延迟时间
    ---@param precise? boolean 是否精确定时
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
    ---@param key KEYBOARD_KEY 按键名或按键扫描码
    function Keyboard:is_key_valid(key)
        if type(key) == "number" then
            for _, v in pairs(Constants.KEYBOARD_SCANCODE) do
                if v == key then
                    return true
                end
            end
        end
        if type(key) == "string" then
            for k, v in pairs(Constants.KEYBOARD_NAME) do
                if v == key then
                    return true
                end
            end
        end
        return false
    end

    ---通过键盘输入由字母和数字构成的字符串序列
    ---@param s string ASCII 字符
    ---@param interval? integer
    ---@param delay? integer
    ---@param precise? boolean
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
    ---@param key KEYBOARD_NAME 可取 `Keyboard.SHIFT`、`Keyboard.CTRL`、`Keyboard.ALT`、`Keyboard.LEFT_SHIFT`、`Keyboard.RIGHT_SHIFT`、`Keyboard.LEFT_CTRL`、`Keyboard.RIGHT_CTRL`、`Keyboard.LEFT_ALT`、`Keyboard.RIGHT_ALT`
    ---@return boolean # 指定修饰键是否按下
    function Keyboard:is_modifier_pressed(key)
        return IsModifierPressed(key)
    end

    ---判断锁定键（CAPSLOCK、SCROLLLOCK、NUMLOCK）是否处于开启状态。
    ---@param key KEYBOARD_NAME 按键名称，可取 `Keyboard.CAPS_LOCK`、`Keyboard.SCROLL_LOCK`、`Keyboard.NUM_LOCK`
    ---@return boolean
    function Keyboard:is_key_lock_on(key)
        return IsKeyLockOn(key)
    end

    Runtime:register_fatal_handler(function()
        Keyboard:reset()
    end)
end -- __KEYBOARD_LUA__
