local Native = {}

local impl = require("executor.Native")

---Get monotomic timestamp, in milliseconds.
---@return integer
function Native.GetMonotonicTimestamp()
    return impl.Native_GetMonotonicTimestamp()
end

---Get system timestamp, in milliseconds.
---@return integer
function Native.GetSystemTimestamp()
    return impl.Native_GetSystemTimestamp()
end

return Native