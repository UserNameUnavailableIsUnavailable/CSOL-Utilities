---@meta
---@module "executor.Native"
local Native = {}

-- Requiring this module releases tracked inputs left by previous VMs.
-- Legacy Logitech globals remain available but bypass validation and tracking.

---Get milliseconds elapsed since hook-owned storage was initialized.
---@return integer
function Native.Time_GetMonotonicTimestamp() return 0 end

---Get milliseconds since the Unix epoch.
---@return integer
function Native.Time_GetSystemTimestamp() return 0 end

---Call the current global Logitech Sleep implementation.
---@param ms integer
function Native.Sleep(ms) end

---Validate all keys before pressing any.
---@param ... integer|string supported Logitech scan codes or names
function Native.Keyboard_Press(...) end

---Validate all keys before releasing any.
---@param ... integer|string supported Logitech scan codes or names
function Native.Keyboard_Release(...) end

---Validate, press and release all keys; remove successful clicks from tracking.
---@param ... integer|string supported Logitech scan codes or names
function Native.Keyboard_Click(...) end

---Release all tracked keys in unspecified order.
function Native.Keyboard_Reset() end

---Return an independent array of tracked keys in unspecified order.
---@return (integer|string)[]
function Native.Keyboard_ListPressed() return {} end

---Query current OS key state via Win32, independently of tracked presses.
---Keys sharing a Win32 virtual key (Enter and numpad Enter) share query state.
---@param key integer|string Logitech scan code/name, or ctrl/shift/alt
---@return boolean
function Native.Keyboard_IsPressed(key) return false end

---Query the toggle state of capslock, numlock or scrolllock.
---@param key integer|string Logitech scan code or lock key name
---@return boolean
function Native.Keyboard_IsLocked(key) return false end

---@param button integer button identifier from 1 to 5
function Native.Mouse_Press(button) end

---@param button integer button identifier from 1 to 5
function Native.Mouse_Release(button) end

---@param button integer button identifier from 1 to 5
function Native.Mouse_Click(button) end

---Release all tracked mouse buttons in unspecified order.
function Native.Mouse_Reset() end

---@return integer[] buttons in unspecified order
function Native.Mouse_ListPressed() return {} end

---@param button integer button identifier from 1 to 5
---@return boolean
function Native.Mouse_IsPressed(button) return false end

---Return normalized Logitech coordinates.
---@return integer x
---@return integer y
function Native.Mouse_GetPosition() return 0, 0 end

---Set normalized coordinates across the virtual desktop via MoveMouseToVirtual.
---@param x integer normalized coordinate from 0 to 65535
---@param y integer normalized coordinate from 0 to 65535
function Native.Mouse_SetPosition(x, y) end

---@param dx integer relative horizontal movement
---@param dy integer relative vertical movement
function Native.Mouse_Move(dx, dy) end

---@param clicks integer wheel movement
function Native.Mouse_Scroll(clicks) end

return Native
