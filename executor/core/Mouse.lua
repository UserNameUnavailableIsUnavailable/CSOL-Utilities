local Native = require "executor.core.Native"
local Runtime  = require "executor.core.Runtime"
local Async    = require "executor.core.Async"

local Mouse = {}

Mouse = {
    LEFT = 1,
    MIDDLE = 2,
    RIGHT = 3,
    BACKWARD = 4,
    FORWARD = 5,
}

local function is_valid_position(x, y)
    return math.type(x) == "integer" and math.type(y) == "integer" and 0 <= x and x < 65536 and 0 <= y and y < 65536
end

local function is_valid_button(btn)
    return btn == Mouse.LEFT or
        btn == Mouse.MIDDLE or
        btn == Mouse.RIGHT or
        btn == Mouse.BACKWARD or
        btn == Mouse.FORWARD
end

---判断某个按钮是否按下。
---@param button MOUSE_BUTTON 按钮值，如 `Mouse.LEFT`。
---@return nil
function Mouse.is_pressed(button)
    if not is_valid_button(button) then
        error("invalid button")
    end
    return Native.Mouse_is_pressed(button)
end

local frozen = false

function Mouse.freeze()
    Native.Mouse_reset()
    frozen = true
end

function Mouse.unfreeze()
    frozen = false
end

function Mouse.is_frozen()
    return frozen
end

---移动鼠标光标到某位置。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param x? integer 横坐标。
---@param y? integer 纵坐标。
---@param delay? integer 移动鼠标光标后的延迟时间。
---@return executor.core.Task
---@nodiscard
function Mouse.set_position(x, y, delay)
    return Async.spawn(function ()
        if not Mouse.is_frozen() then
            Native.Mouse_set_position(x, y)
        end
        Runtime.sleep(delay or 10):await()
    end)
end

---获取鼠标光标位置
---@return integer x, integer y 横、纵坐标。
---@nodiscard
function Mouse.get_position()
    return Native.Mouse_get_position()
end

--双击延迟
---@type integer
Mouse.DOUBLE_CLICK_INTERVAL = 150

---相对移动鼠标光标。当 `Runtime:is_paused` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param rightward? integer 向右移动的偏移量，负数表示向左。
---@param downward? integer 向下移动的偏移量，负数表示向上。
---@param delay? integer 移动光标后的延迟。
function Mouse.move(rightward, downward, delay)
    rightward, downward = rightward or 0, downward or 0
    delay = delay or 10
    return Async.spawn(function ()
        if not Mouse.is_frozen() then
            Native.Mouse_move(rightward, downward)
        end
        Runtime.sleep(delay):await()
    end)
end

---按下按钮。
---@param button MOUSE_BUTTON
---@param delay? integer 按下某个按钮后的延迟时间。
---@return executor.core.Task
---@nodiscard
function Mouse.press(button, delay)
    delay = delay or 10
    if not is_valid_button(button) then
        error("invalid button")
    end
    return Async.spawn(function ()
        if not Mouse.is_frozen() then
            Native.Mouse_press(button)
        end
        Runtime.sleep(delay):await()
    end)
end

---弹起按钮。
---@param button MOUSE_BUTTON 按钮值，如 `Mouse.LEFT`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay? integer 释放某个按钮后的延迟时间。
---@return executor.core.Task
---@nodiscard
function Mouse.release(button, delay)
    delay = delay or 10
    if not is_valid_button(button) then
        error("invalid button")
    end
    return Async.spawn(function ()
        if not Mouse.is_frozen() then
            Native.Mouse_release(button)
        end
        Runtime.sleep(delay):await()
    end)
end

---单击一次按钮。
---@param button MOUSE_BUTTON 按钮值，如 `Mouse.LEFT`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay? integer 单击某个按钮后的延迟时间，单位为毫秒。
---@return executor.core.Task
---@nodiscard
function Mouse.click(button, delay)
    delay = delay or 10
    if not is_valid_button(button) then
        error("invalid button")
    end
    return Async.spawn(function ()
        if not Mouse.is_frozen() then
            Native.Mouse_click(button)
        end
        Runtime.sleep(delay):await()
    end)
end

---双击一次按钮。
---@param button MOUSE_BUTTON 按钮值，如 `Mouse.LEFT`。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param delay? integer 双击后的延迟时间，单位为毫秒。
---@return executor.core.Task
---@nodiscard
function Mouse.double_click(button, delay)
    if not is_valid_button(button) then
        error("invalid button")
    end
    return Async.spawn(function ()
        delay = delay or 10
        if not Mouse.is_frozen() then
            Native.Mouse_click(button)
            Runtime.sleep(Mouse.DOUBLE_CLICK_INTERVAL):await()
            Native.Mouse_click(button)
        end
        Runtime.sleep(delay):await()
    end)
end

---使用鼠标单击屏幕上某个位置。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param button MOUSE_BUTTON 鼠标按钮。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@param delay? integer 点击后的延时
---@see Mouse.locate 获取 `(x, y)` 。
---@return executor.core.Task
---@nodiscard
function Mouse.click_on(button, x, y, delay)
    if not is_valid_button(button) then
        error("invalid button")
    end
    if not is_valid_position(x, y) then
        error("invalid position")
    end
    delay = delay or 50
    return Async.spawn(function ()
        Mouse.set_position(x, y):await()
        Mouse.click(button, delay):await()
    end)
end

---使用鼠标双击屏幕上某个位置。当 `self:is_frozen()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param button MOUSE_BUTTON 鼠标按钮。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@param delay? integer 点击后的延时
---@see Mouse.locate 获取 `(x, y)` 。
---@return executor.core.Task
---@nodiscard
function Mouse.double_click_on(button, x, y, delay)
    if not is_valid_button(button) then
        error("invalid button")
    end
    if not is_valid_position(x, y) then
        error("invalid position")
    end
    delay = delay or 50
    return Async.spawn(function ()
        Mouse.set_position(x, y):await()
        Mouse.double_click(button, delay):await()
    end)
end

---重复点击鼠标按钮若干次。
---@param button MOUSE_BUTTON
---@param times integer
---@param interval? integer
---@param delay? integer
---@return executor.core.Task
---@nodiscard
function Mouse:click_several_times(button, times, interval, delay)
    if not is_valid_button(button) then
        error("invalid button")
    end
    interval = interval or 10
    delay = delay or 50
    return Async.spawn(function ()
        if not self:is_frozen() then
            for i = 1, times do
                Native.Mouse_click(button)
                if i ~= times then
                    Runtime.sleep(interval):await()
                end
            end
        end
        Runtime.sleep(delay):await()
    end)
end

---使用鼠标重复点击屏幕上的某个位置若干次。当 `Runtime.is_paused()` 为 `true` 时，该函数将直接返回，不进行任何操作。
---@param button MOUSE_BUTTON 鼠标按钮。
---@param x integer 横坐标。
---@param y integer 纵坐标。
---@param times integer 重复次数。
---@param interval? integer 间隔时间。
---@param delay? integer 重复点击动作完成后的延迟时间，单位为毫秒。
---@return executor.core.Task
---@nodiscard
function Mouse:click_several_times_on(button, x, y, times, interval, delay)
    if not is_valid_button(button) then
        error("invalid button")
    end
    if not is_valid_position(x, y) then
        error("invalid position")
    end
    interval = interval or 50
    delay = delay or 50
    return Async.spawn(function ()
        Mouse.set_position(x, y):await()
        if not self:is_frozen() then
            for i = 1, times do
                Native.Mouse_click(button)
                if i ~= times then
                    Runtime.sleep(interval):await()
                end
            end
        end
        Runtime.sleep(delay):await()
    end)
end

---弹起所有通过 `Mouse:press` 按下但未通过 `Mouse.release` 回弹的按钮。
function Mouse:reset()
    Native.Mouse_reset()
end

---滚动鼠标滚轮。
---@param times integer 滚动次数，正数表示向上滚动，负数表示向下滚动。
---@param delay? integer 滚动后的延迟，单位为毫秒。
---@return executor.core.Task
---@nodiscard
function Mouse:scroll(times, delay)
    delay = delay or 10
    return Async.spawn(function ()
        if not self:is_frozen() and math.abs(times) > 0 then
            Native.Mouse_scroll(times)
        end
        Runtime.sleep(delay):await()
    end)
end

---检测鼠标光标是否被锁定在某个位置。
---@return executor.core.Task<boolean>
---@nodiscard
function Mouse:is_cursor_position_locked()
    return Async.spawn(function ()
        Mouse.set_position(32767, 32767):await()
        local before_x, before_y = Mouse.get_position()
        Mouse.move(400, 400, 25):await() -- 尝试移动鼠标
        local after_x, after_y = self:locate()
        Mouse.move(-400, -400, 25):await() -- 移回原位置
        if math.abs(after_x - before_x) <= 300 and math.abs(after_y - before_y) <= 300 then
            return true
        end
        return false
    end)
end

return Mouse