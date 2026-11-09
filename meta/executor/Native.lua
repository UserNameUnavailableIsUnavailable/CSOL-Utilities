---@meta
---@module "executor.Native"
local Native = {}

---Get system timestamp.
---@return integer
function Native.GetSystemTimepoint()
    return 0
end

---Get monotonic time.
---@return integer
function Native.GetMonotonicTimepoint()
    return 0
end

---The native sleep will block the execution atomically for specified time. In most cases this may not be what you want becasue interruption is impossible during sleep (e.g., "save & run" will not be responded). For most cases, consider `executor.Logitech.Sleep()` instead.
---@see executor.Logitech.Sleep
---@param ms integer
function Native.Sleep(ms)
end

return Native