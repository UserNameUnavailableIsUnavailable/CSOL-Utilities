---@meta
---@module "executor.core.Native"
local Native = {}

-- Requiring this module releases tracked inputs left by previous VMs.
-- Legacy Logitech globals remain available but bypass validation and tracking.

---Get milliseconds elapsed since hook-owned storage was initialized.
---@return integer
function Native.Time_get_monotonic_timestamp() return 0 end

---Get milliseconds since the Unix epoch.
---@return integer
function Native.Time_get_system_timestamp() return 0 end

---Call the current global Logitech Sleep implementation.
---@param ms integer
function Native.Raw_sleep(ms) end

---Get current thread id.
---@return integer
function Native.Raw_get_thread_id()
    return 0
end

---Attach a fresh async runtime after Runtime_close. Repeated attachment is idempotent.
function Native.Runtime_open() end

---Queue a timer completion for a task id. An id may have only one pending timer.
---@param task_id integer nonnegative task identifier
---@param milliseconds integer from 0 to 2147483647
function Native.Runtime_sleep(task_id, milliseconds) end

---Poll IOCP; return each completed task id once. No pending I/O returns immediately.
---@param timeout? integer -1 (default) waits; 0 polls; positive values bound the wait
---@return integer[] ready_task_ids
function Native.Runtime_poll(timeout) return {} end

---Stop timers, join callbacks, discard pending completions and close IOCP.
function Native.Runtime_close() end

---Validate all keys before pressing any.
---@param ... integer|string supported Logitech scan codes or names
function Native.Keyboard_press(...) end

---Validate all keys before releasing any.
---@param ... integer|string supported Logitech scan codes or names
function Native.Keyboard_release(...) end

---Validate, press and release all keys; remove successful clicks from tracking.
---@param ... integer|string supported Logitech scan codes or names
function Native.Keyboard_click(...) end

---Release all tracked keys in unspecified order.
function Native.Keyboard_reset() end

---Return an independent array of tracked keys in unspecified order.
---@return (integer|string)[]
function Native.Keyboard_list_pressed() return {} end

---Query current OS key state via Win32, independently of tracked presses.
---Keys sharing a Win32 virtual key (Enter and numpad Enter) share query state.
---@param key integer|string Logitech scan code/name, or ctrl/shift/alt
---@return boolean
function Native.Keyboard_is_pressed(key) return false end

---Query the toggle state of capslock, numlock or scrolllock.
---@param key integer|string Logitech scan code or lock key name
---@return boolean
function Native.Keyboard_is_locked(key) return false end

---@param button integer button identifier from 1 to 5
function Native.Mouse_press(button) end

---@param button integer button identifier from 1 to 5
function Native.Mouse_release(button) end

---@param button integer button identifier from 1 to 5
function Native.Mouse_click(button) end

---Release all tracked mouse buttons in unspecified order.
function Native.Mouse_reset() end

---@return integer[] buttons in unspecified order
function Native.Mouse_list_pressed() return {} end

---@param button integer button identifier from 1 to 5
---@return boolean
function Native.Mouse_is_pressed(button) return false end

---Return normalized Logitech coordinates.
---@return integer x
---@return integer y
function Native.Mouse_get_position() return 0, 0 end

---Set normalized coordinates across the virtual desktop via MoveMouseToVirtual.
---@param x integer normalized coordinate from 0 to 65535
---@param y integer normalized coordinate from 0 to 65535
function Native.Mouse_set_position(x, y) end

---@param dx integer relative horizontal movement
---@param dy integer relative vertical movement
function Native.Mouse_move(dx, dy) end

---@param clicks integer wheel movement
function Native.Mouse_scroll(clicks) end

return Native
