--- 该文件包含从 Logitech G Hub 中导出或在 Executor 模块中定义的的基本函数、变量的声明，用于消除语言服务器发出的符号缺失警告，因此 **请勿** 将其包含在任何模块中。

--- This file contains declarations for basic functions, variables exported from Logitech G Hub or implemented in Executor modules and is used for eliminating symbol-missing warnings emitted by the language server, so **NEVER** include it in any module.

error("This file is a declaration file and should not be included in any module.")

---Search path for modules.
---@type string
---@see Include
PATH = ""

---Include a module from `PATH`. When called, `PATH` will be added before `path`.
---@param path string path of the module to `PATH`
---@see PATH
function Include(path) end

---Command Identifier
---@type integer
CmdId = 0

---Command type
---@type integer
CmdType = Command.CMD_NOP

---Command timepoint
---@type integer
CmdTimepoint = 0

---Whether the command is repeatable
---@type boolean
CmdRepeatable = false

---Sleep for a specified amount of time. The sleeping time is not guaranteed to be accurate.
---@param milliseconds integer
function Sleep(milliseconds) end

---Output a log message.
---@param ... any formatted string containing the message
function OutputLogMessage(...) end

---Get the running time of the script since loading.
---@nodiscard
---@return integer
function GetRunningTime()
    return 0
end

---Get the current date and time. This function is a mirror of `os.date()`.
---@param format? string
---@param time? integer
---@return string|osdate
---@nodiscard
---@see os.date
function GetDate(format, time)
    return os.date(format, time)
end

---Clears the output window of the script editor.
function ClearLog() end

---Simulate a keyboard key press.
---@param ... integer|string scan code(s) or name(s) of the key(s) to press
function PressKey(...) end

---Simulate a keyboard key release.
---@param ... integer|string scan code(s) or name(s) of the key(s) to release
function ReleaseKey(...) end

---Simulate a keyboard key press followed by a release.
---@param ... integer|string scan code(s) or name(s) of the key(s) to press and release
function PressAndReleaseKey(...) end

---Determine if a particular modifier key is currently in a pressed state.
---@param key_name string the predefined keyname of the modifier key to be pressed
---@return boolean
function IsModifierPressed(key_name) return false end

---Simulate a mouse button press.
---@param button integer button identifier
function PressMouseButton(button) end

---Simulate a mouse button release.
---@param button integer button identifier
function ReleaseMouseButton(button) end

---Simulate a mouse button press followed by a release.
---@param button integer button identifier
function PressAndReleaseMouseButton(button) end

---Determine if a particular mouse button is currently in a pressed state
---@param button integer button identifier
---@return boolean
function IsMouseButtonPressed(button) return false end

---Move the mouse cursor to an absolute position on the screen.
---@param x integer
---@param y integer
function MoveMouseTo(x, y) end

---Simulate mouse wheel movement.
---@param click integer
function MoveMouseWheel(click) end

---Simulate relative mouse movement.
---@param dx integer
---@param dy integer
function MoveMouseRelative(dx, dy) end

---Move the mouse cursor to an absolute position on a multi-monitor screen layout
---@param x integer
---@param y integer
function MoveMouseToVirtual(x, y) end

---Get the normalized coordinates of the current mouse cursor location.
---@return integer, integer
function GetMousePosition()
    return 0, 0
end

---Determine if a particular lock button is currently in an enabled state.
---@param key_name string the predefined keyname of the lock button to be checked
---@return boolean
function IsKeyLockOn(key_name) return false end

---Send log messages to the Windows debugger.
---@param ... any formatted string containing the message
function OutputDebugMessage(...) end
