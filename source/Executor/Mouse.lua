if (not Mouse_lua)
then
Include("Context.lua")
Include("Runtime.lua")
Mouse_lua = true
---@class Mouse
---@field LEFT integer 鼠标左键。
---@field MIDDLE integer 鼠标中键。
---@field RIGHT integer 鼠标右键。
---@field BACKWARD integer 鼠标侧键（后退）。
---@field FORWARD integer 鼠标侧键（前进）。
---@field DOUBLE_CLICK_INTERVAL integer 双击时间间隔150ms（人手实际双击间隔时间一般为此值）
---@field unreleased table 鼠标按下但未释放的按钮，`true` 表示按下。
Mouse = {
    LEFT = 1, MIDDLE = 2 ,RIGHT = 3, BACKWARD = 4, FORWARD = 5,
    DOUBLE_CLICK_INTERVAL = 150,
}

---@type {[integer]: boolean}
Mouse.unreleased = {}

---获取鼠标光标位置
---@return integer x, integer y 横、纵坐标。
function Mouse:locate_cursor()
    local x, y = GetMousePosition()
    return x, y
end

---移动鼠标光标到某位置。当 `Runtime:is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@param delay integer | nil 移动鼠标光标后的延迟时间，默认为 `Delay.SHORT`。
---@return nil
function Mouse:move_cursor_to(x, y, delay)
    if (Runtime:is_paused()) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    MoveMouseTo(x, y)
    delay = delay or Delay.SHORT
    Runtime:sleep(delay)
end

---按下按钮。
---@param button integer
---@param delay integer | nil 按下某个按钮后的延迟时间，默认为 `Delay.SHORT`。
---@return nil
function Mouse:press(button, delay)
    if (Runtime:is_paused())
    then
        return
    end
    PressMouseButton(button)
    self.unreleased[button] = true
    delay = delay or Delay.SHORT
    Runtime:sleep(delay)
end

---弹起按钮。
---@param button integer 按钮值，如 `Mouse.LEFT`。当 `Runtime:is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay integer | nil 释放某个按钮后的延迟时间，默认为 `Delay.SHORT`。
---@return nil
function Mouse:release(button, delay)
    if (Runtime:is_paused()) -- 当下达退出指令时，不进行任何操作。
    then
        return
    end
    ReleaseMouseButton(button)
    self.unreleased[button] = nil
    delay = delay or Delay.SHORT
    Runtime:sleep(delay)
end

---判断某个按钮是否按下。
---@param button integer 按钮值，如 `Mouse.LEFT`。
---@return nil
function Mouse:is_pressed (button)
    return IsMouseButtonPressed(button)
end

---弹起所有通过 `Mouse:press` 按下但未通过 `Mouse.release` 回弹的按钮（记录在 `Mouse.unreleased` 中）。当 `Runtime:is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay integer | nil 释放每个按钮后的延迟时间，默认为 `Delay.SHORT`。
---@return nil
function Mouse:release_all (delay)
    if (Runtime:is_paused()) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    delay = delay or Delay.SHORT
    for button, _ in pairs(self.unreleased)
    do
        ReleaseMouseButton(button)
        self.unreleased[button] = nil
        Runtime:sleep(delay)
    end
end

---单击一次按钮。
---@param button integer 按钮值，如 `Mouse.LEFT`。当 `Runtime:is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay integer | nil 单击某个按钮后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
---@return nil
function Mouse:click(button, delay)
    if (Runtime:is_paused()) -- 当下达退出指令时，不进行任何操作。
    then
        return
    end
    PressAndReleaseMouseButton(button)
    delay = delay or Delay.SHORT
    Runtime:sleep(delay)
end

---双击一次按钮。
---@param button integer 按钮值，如 `Mouse.LEFT`。当 `Runtime:is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay integer | nil 双击后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
---@return nil
function Mouse:double_click(button, delay)
    if (Runtime:is_paused()) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    PressAndReleaseMouseButton(button)
    Sleep(Mouse.DOUBLE_CLICK_INTERVAL)
    PressAndReleaseMouseButton(button)
    delay = delay or Delay.SHORT
    Runtime:sleep(delay)
end

---使用鼠标单击屏幕上某个位置。当 `Runtime:is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@param delay integer | nil 点击后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
---@see Mouse.locate_cursor 获取 `(x, y)` 。
---@return nil
function Mouse:click_on(x, y, delay)
    if (Runtime:is_paused()) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    Mouse:move_cursor_to(x, y, Delay.SHORT)
    delay = delay or Delay.SHORT
    Mouse:click(Mouse.LEFT, delay)
end

---相对移动鼠标光标。当 `Runtime:is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param rightward integer 向右移动的偏移量，负数表示向左。
---@param downward integer 向下移动的偏移量，负数表示向上。
---@param delay integer | nil 移动光标后的延迟，默认为 `Delay.SHORT`。
---@return nil
---@remark 
function Mouse:move_relative(rightward, downward, delay)
    if (Runtime:is_paused()) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    MoveMouseRelative(math.ceil(rightward), math.ceil(downward))
    delay = delay or Delay.SHORT
    Runtime:sleep(delay)
end

---使用鼠标双击屏幕上某个位置。当 `Runtime:is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@param delay integer | nil 双击后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
---@return nil
---@see Mouse.locate_cursor 获取 `(x, y)` 。
function Mouse:double_click_on(x, y, delay)
    if (Runtime:is_paused()) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    Mouse:move_cursor_to(x, y, Delay.SHORT)
    delay = delay or Delay.SHORT
    Mouse:double_click(Mouse.LEFT, delay)
end

---使用鼠标重复点击屏幕上的某个位置若干次。当 `Runtime.is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@param n integer 重复次数。
---@param delay integer | nil 重复点击动作完成后的延迟时间，单位为毫秒，默认为 `Delay.SHORT`。
function Mouse:click_on_several_times(x, y, n, delay)
    if (Runtime:is_paused())
    then
        return
    end
    if (type(n) ~= "number")
    then
        return
    end
    n = math.floor(n) or 0
    while (n > 0)
    do
        Mouse:click_on(x, y, Delay.NORMAL)
        n = n - 1
    end
    delay = delay or Delay.SHORT
    Runtime:sleep(delay)
end


Runtime:register_context(
    Context:new(
        function (self)
            for button, _ in pairs(Mouse.unreleased)
            do
                Mouse:release(button)
                self.storage[button] = true
            end
        end,
        function (self)
            for button, _ in pairs(self.storage --[=[@as {[integer]: boolean}]=])
            do
                Mouse:press(button)
                self.storage[button] = nil
            end
        end
    )
)

end -- if (not Mouse)