---@meta
---@module "executor.Logitech"

local Logitech = {}

---Sleep for a specified amount of time. The sleeping time is not guaranteed to be accurate.

---@param milliseconds integer
function Logitech.Sleep(milliseconds)
end

---Output a log message.
---@param fmt string format string
---@param ... any formatted string containing the message
function Logitech.OutputLogMessage(fmt, ...)
end

---Get the running time of the script since loading.
---@nodiscard
---@return integer
function Logitech.GetRunningTime()
    return 0
end

---Get the current date and time. This function is a mirror of `os.date()`.
---@param format? string
---@param time? integer
---@return string|osdate
---@nodiscard
---@see os.date
function Logitech.GetDate(format, time)
    return os.date(format, time)
end

---Clears the output window of the script editor.
function Logitech.ClearLog() end

---Simulate a keyboard key press.
---@param ... integer|string scan code(s) or name(s) of the key(s) to press
function Logitech.PressKey(...) end

---Simulate a keyboard key release.
---@param ... integer|string scan code(s) or name(s) of the key(s) to release
function Logitech.ReleaseKey(...) end

---Simulate a keyboard key press followed by a release.
---@param ... integer|string scan code(s) or name(s) of the key(s) to press and release
function Logitech.PressAndReleaseKey(...) end

---Determine if a particular modifier key is currently in a pressed state.
---@param key_name string the predefined keyname of the modifier key to be pressed
---@return boolean
function Logitech.IsModifierPressed(key_name) return false end

---Simulate a mouse button press.
---@param button integer button identifier
function Logitech.PressMouseButton(button) end

---Simulate a mouse button release.
---@param button integer button identifier
function Logitech.ReleaseMouseButton(button) end

---Simulate a mouse button press followed by a release.
---@param button integer button identifier
function Logitech.PressAndReleaseMouseButton(button) end

---Determine if a particular mouse button is currently in a pressed state
---@param button integer button identifier
---@return boolean
function Logitech.IsMouseButtonPressed(button) return false end

---Move the mouse cursor to an absolute position on the screen.
---@param x integer
---@param y integer
function Logitech.MoveMouseTo(x, y) end

---Simulate mouse wheel movement.
---@param click integer
function Logitech.MoveMouseWheel(click) end

---Simulate relative mouse movement.
---@param dx integer
---@param dy integer
function Logitech.MoveMouseRelative(dx, dy) end

---Move the mouse cursor to an absolute position on a multi-monitor screen layout
---@param x integer
---@param y integer
function Logitech.MoveMouseToVirtual(x, y) end

---Get the normalized coordinates of the current mouse cursor location.
---@return integer, integer
function Logitech.GetMousePosition()
    return 0, 0
end

---Determine if a particular lock button is currently in an enabled state.
---@param key_name string the predefined keyname of the lock button to be checked
---@return boolean
function Logitech.IsKeyLockOn(key_name) return false end

---Send log messages to the Windows debugger.
---@param ... any formatted string containing the message
function Logitech.OutputDebugMessage(...) end

return Logitech