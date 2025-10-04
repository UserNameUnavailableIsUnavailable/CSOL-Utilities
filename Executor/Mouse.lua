if not Mouse_lua then
    Mouse_lua = true

    Include("Emulator.lua")
    Include("Context.lua")
    Include("Runtime.lua")
    Include("Error.lua")
    Include("Delay.lua")
    Include("Version.lua")
    Version:set("Mouse", "1.5.2")

    ---@class Mouse
    ---@field LEFT integer 鼠标左键。
    ---@field MIDDLE integer 鼠标中键。
    ---@field RIGHT integer 鼠标右键。
    ---@field BACKWARD integer 鼠标侧键（后退）。
    ---@field FORWARD integer 鼠标侧键（前进）。
    ---@field DOUBLE_CLICK_INTERVAL integer 双击时间间隔150ms（人手实际双击间隔时间一般为此值）
    ---@field unreleased table 鼠标按下但未释放的按钮，`true` 表示按下。
    Mouse = {
        LEFT = Constants.mouse_buttons.LEFT,
        MIDDLE = Constants.mouse_buttons.MIDDLE,
        RIGHT = Constants.mouse_buttons.RIGHT,
        BACKWARD = Constants.mouse_buttons.BACK,
        FORWARD = Constants.mouse_buttons.FORWARD,
    }

    local valid_mouse_buttons = {}
    for k, v in pairs(Mouse) do
        valid_mouse_buttons[k] = v
    end

    Mouse.frozen = false

    function Mouse:freeze()
        self.frozen = true
    end

    function Mouse:unfreeze()
        self.frozen = false
    end

    function Mouse:is_frozen()
        return self.frozen
    end

    function Mouse:is_position_valid(x, y)
        return math.type(x) == "integer" and math.type(y) == "integer" and 0 <= x and x < 65536 and 0 <= y and y < 65536
    end

    --检查按钮名是否有效。
    ---@param button_name any
    ---@return boolean # 有效返回 `true`，无效返回 `false`。
    function Mouse:is_button_value_valid(button_name)
        if math.type(button_name) ~= "integer" then
            return false
        end
        for _, v in pairs(valid_mouse_buttons) do
            if v == button_name then
                return true
            end
        end
        return false
    end

    --双击延迟
    ---@type integer
    Mouse.DOUBLE_CLICK_INTERVAL = 150

    ---@type {[integer]: boolean}
    Mouse.unreleased = {}

    ---获取鼠标光标位置
    ---@return integer x, integer y 横、纵坐标。
    function Mouse:locate()
        return GetMousePosition()
    end

    ---移动鼠标光标到某位置。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param x integer|nil 横坐标。
    ---@param y integer|nil 纵坐标。
    ---@param delay integer|nil 移动鼠标光标后的延迟时间，默认为 `Delay.SHORT`。
   ---@param precise boolean|nil 是否精确定时
    function Mouse:place(x, y, delay, precise)
        delay = delay or Delay.SHORT
        if not self:is_frozen() and self:is_position_valid(x, y) then
            MoveMouseToVirtual(x--[[@as integer]], y--[[@as integer]])
        end
        Runtime:sleep(delay, precise)
    end

    ---判断某个按钮是否按下。
    ---@param button integer 按钮值，如 `Mouse.LEFT`。
    ---@return nil
    function Mouse:is_pressed(button)
        if not self:is_button_value_valid(button) then
            return false
        end
        return IsMouseButtonPressed(button)
    end

    ---相对移动鼠标光标。当 `Runtime:is_paused` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param rightward integer|nil 向右移动的偏移量，负数表示向左。
    ---@param downward integer|nil 向下移动的偏移量，负数表示向上。
    ---@param delay integer|nil 移动光标后的延迟，默认为 `Delay.SHORT`。
    ---@param precise boolean|nil 是否精确定时。
    function Mouse:move_relative(rightward, downward, delay, precise)
        rightward, downward = rightward or 0, downward or 0
        if not self:is_frozen() and math.type(rightward) == "integer" and math.type(downward) == "integer" then
            MoveMouseRelative(rightward, downward)
        end
        Runtime:sleep(delay or Delay.SHORT, precise)
    end

    ---按下按钮。
    ---@param button integer
    ---@param delay integer|nil 按下某个按钮后的延迟时间，默认为 `Delay.SHORT`。
    ---@param precise boolean|nil 是否精确定时
    function Mouse:press(button, delay, precise)
        delay = delay or Delay.SHORT
        if not self:is_frozen() and Mouse:is_button_value_valid(button) then
            PressMouseButton(button)
            self.unreleased[button] = true
        end
        Runtime:sleep(delay, precise)
    end

    ---弹起按钮。
    ---@param button integer 按钮值，如 `Mouse.LEFT`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay integer|nil 释放某个按钮后的延迟时间，默认为 `Delay.SHORT`。
    ---@param precise boolean|nil 是否精确定时
    ---@return nil
    function Mouse:release(button, delay, precise)
        delay = delay or Delay.SHORT
        if not self:is_frozen() and self:is_button_value_valid(button) then
            ReleaseMouseButton(button)
            self.unreleased[button] = nil
        end
        Runtime:sleep(delay, precise)
    end

    ---单击一次按钮。
    ---@param button integer 按钮值，如 `Mouse.LEFT`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay integer|nil 单击某个按钮后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
    ---@param precise boolean|nil 是否精确定时
    function Mouse:click(button, delay, precise)
        delay = delay or Delay.SHORT
        if not self:is_frozen() and Mouse:is_button_value_valid(button) then
            PressAndReleaseMouseButton(button)
        end
        Runtime:sleep(delay, precise)
    end

    ---双击一次按钮。
    ---@param button integer 按钮值，如 `Mouse.LEFT`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param delay integer|nil 双击后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
    ---@param precise boolean|nil 是否精确定时
    function Mouse:double_click(button, delay, precise)
        delay = delay or Delay.SHORT
        if not self:is_frozen() and Mouse:is_button_value_valid(button) then
            PressAndReleaseMouseButton(button)
            Runtime:sleep(Mouse.DOUBLE_CLICK_INTERVAL, precise)
            PressAndReleaseMouseButton(button)
        end
        Runtime:sleep(delay, precise)
    end

    ---使用鼠标单击屏幕上某个位置。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param button integer 鼠标按钮。
    ---@param x integer|nil 横坐标。
    ---@param y integer|nil 纵坐标。
    ---@param delay integer|nil 点击后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
    ---@param precise boolean|nil 是否精确定时
    ---@see Mouse.locate 获取 `(x, y)` 。
    function Mouse:click_on(button, x, y, delay, precise)
        delay = delay or Delay.SHORT
        if not self:is_frozen() and self:is_button_value_valid(button) and self:is_position_valid(x, y) then
            MoveMouseToVirtual(x--[[@as integer]], y--[[@as integer]])
            Runtime:sleep(Delay.SHORT, precise)
            PressAndReleaseMouseButton(button)
        end
        Runtime:sleep(delay, precise)
    end

    ---使用鼠标双击屏幕上某个位置。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param button integer 鼠标按钮。
    ---@param x integer 横坐标。
    ---@param y integer 纵坐标。
    ---@param delay integer|nil 双击后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
    ---@param precise boolean|nil 是否精确定时
    ---@see Mouse.locate 获取 `(x, y)` 。
    function Mouse:double_click_on(button, x, y, delay, precise)
        delay = delay or Delay.SHORT
        if not self:is_frozen() and self:is_button_value_valid(button) and self:is_position_valid(x, y) then
            MoveMouseToVirtual(x, y)
            Runtime:sleep(Delay.SHORT, precise)
            PressAndReleaseMouseButton(button)
            Runtime:sleep(self.DOUBLE_CLICK_INTERVAL, precise)
            PressAndReleaseMouseButton(button)
        end
        Runtime:sleep(delay, precise)
    end

    ---重复点击鼠标按钮若干次。
    ---@param button integer
    ---@param times integer|nil
    ---@param interval integer|nil
    ---@param delay integer|nil
    ---@param precise boolean|nil
    function Mouse:click_several_times(button, times, interval, delay, precise)
        interval = interval or Delay.SHORT
        delay = delay or Delay.SHORT
        times = times or 0
        if not self:is_frozen() and math.type(times) == "integer" and Mouse:is_button_value_valid(button) then
            for i = 1, times do
                PressAndReleaseMouseButton(button)
                if i ~= times then
                    Runtime:sleep(interval, precise)
                end
            end
        end
        Runtime:sleep(delay, precise)
    end

    ---使用鼠标重复点击屏幕上的某个位置若干次。当 `Runtime.is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    ---@param button integer 鼠标按钮。
    ---@param x integer|nil 横坐标。
    ---@param y integer|nil 纵坐标。
    ---@param times integer|nil 重复次数。
    ---@param interval integer|nil 间隔时间。
    ---@param delay integer|nil 重复点击动作完成后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
    ---@param precise boolean|nil 是否精确定时
    function Mouse:click_several_times_on(button, x, y, times, interval, delay, precise)
        interval = interval or Delay.SHORT
        delay = delay or Delay.SHORT
        times = times or 0
        if
            not self:is_frozen()
            and math.type(times) == "integer"
            and self:is_button_value_valid(button)
            and self:is_position_valid(x, y)
        then
            MoveMouseToVirtual(x--[[@as integer]], y--[[@as integer]])
            Runtime:sleep(Delay.SHORT, precise)
            for i = 1, times do
                PressAndReleaseMouseButton(button)
                if i ~= times then
                    Runtime:sleep(interval, precise)
                end
            end
        end
        Runtime:sleep(delay, precise)
    end

    ---弹起所有通过 `Mouse:press` 按下但未通过 `Mouse.release` 回弹的按钮（记录在 `Mouse.unreleased` 中）。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
    function Mouse:reset()
        for button, _ in pairs(self.unreleased) do
            ReleaseMouseButton(button)
            Runtime:sleep(Delay.MINI)
            self.unreleased[button] = nil
        end
    end

    ---滚动鼠标滚轮。
    ---@param times integer|nil 滚动次数，正数表示向上滚动，负数表示向下滚动。
    ---@param delay integer|nil 滚动后的延迟，单位为毫秒。
    ---@param precise boolean|nil 是否精确定时
    function Mouse:roll(times, delay, precise)
        times = times or 0
        delay = delay or Delay.SHORT
        if not self:is_frozen() and math.type(times) == "integer" then
            MoveMouseWheel(times)
        end
        Runtime:sleep(delay, precise)
    end

    ---检测鼠标光标是否被锁定在某个位置。
    ---@return boolean
    function Mouse:is_cursor_position_locked()
        self:place(32767, 32767, 25, true)
        local before_x, before_y = self:locate()
        self:move_relative(400, 400, 25, true) -- 尝试移动鼠标
        local after_x, after_y = self:locate()
        self:move_relative(-400, -400, 25, true) -- 移回原位置
        if math.abs(after_x - before_x) <= 300 and math.abs(after_y - before_y) <= 300 then
            return true
        end
        return false
    end

    Error:register_fatal_disposal(function()
        Mouse:reset()
    end)
end -- Mouse_lua
