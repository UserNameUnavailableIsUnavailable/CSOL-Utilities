Load(PATH .. "Runtime.lua")
Load(PATH .. "Delay.lua")
if (not Mouse)
then

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

---移动鼠标光标到某位置。当 `Runtime.exit` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@param delay integer | nil 移动鼠标光标后的延迟时间。
---@return nil
function Mouse:move_cursor_to(x, y, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    MoveMouseTo(x, y)
    Runtime:sleep(delay)
end

---按下按钮。
---@param button integer
---@param delay integer | nil 按下某个按钮后的延迟时间。
---@return nil
function Mouse:press(button, delay)
    if (Runtime.exit)
    then
        return
    end
    PressMouseButton(button)
    self.unreleased[button] = true
    Runtime:sleep(delay)
end

---弹起按钮。
---@param button integer 按钮值，如 `Mouse.LEFT`。当 `Runtime.exit` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay integer | nil 释放某个按钮后的延迟时间。
---@return nil
function Mouse:release(button, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作。
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

---判断某个按钮是否按下。
---@param button integer 按钮值，如 `Mouse.LEFT`。
---@return nil
function Mouse:is_pressed (button)
    return IsMouseButtonPressed(button)
end

---弹起所有通过 `Mouse:press` 按下但未通过 `Mouse.release` 回弹的按钮（记录在 `Mouse.unreleased` 中）。当 `Runtime.exit` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay integer | nil 释放每个按钮后的延迟时间
---@return nil
function Mouse:release_all (delay)
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

---单击一次按钮。
---@param button integer 按钮值，如 `Mouse.LEFT`。当 `Runtime.exit` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay integer | nil 点击某个按钮后的延迟时间。
---@return nil
function Mouse:click(button, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作。
    then
        return
    end
    PressAndReleaseMouseButton(button)
    Runtime:sleep(delay)
end

---双击一次按钮。
---@param button integer 按钮值，如 `Mouse.LEFT`。当 `Runtime.exit` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay integer | nil 双击某个按钮后的延迟时间。
---@return nil
function Mouse:double_click(button, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    PressAndReleaseMouseButton(button)
    Sleep(Mouse.DOUBLE_CLICK_INTERVAL)
    PressAndReleaseMouseButton(button)
    Runtime:sleep(delay)
end

---使用鼠标单击屏幕上某个位置。当 `Runtime.exit` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@see Mouse.locate 获取 `(x, y)` 。
---@return nil
function Mouse:click_on(x, y)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    Mouse:move_cursor_to(x, y, Delay.SHORT)
    Mouse:click(Mouse.LEFT, Delay.SHORT)
end

---相对移动鼠标光标。当Runtime.exit为true时，该函数将直接返回，不进行任何操作。
---@param rightward integer 向右移动的偏移量，负数表示向左。
---@param downward integer 向下移动的偏移量，负数表示向上。
---@param delay integer | nil 移动光标后的延迟
---@return nil
---@remark 
function Mouse:moveRelative(rightward, downward, delay)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    MoveMouseRelative(math.ceil(rightward), math.ceil(downward))
    Runtime:sleep(delay)
end

---使用鼠标双击屏幕上某个位置。当 `Runtime.exit` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@return nil
---@see Mouse.locate 获取 `(x, y)` 。
function Mouse:double_click_on(x, y)
    if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
    then
        return
    end
    Mouse:move_cursor_to(x, y, Delay.SHORT)
    Mouse:double_click(Mouse.LEFT, Delay.SHORT)
end

end -- if (not Mouse)