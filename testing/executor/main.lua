local Native = require("executor.core.Native")
local Logitech = require("executor.Logitech")

print(Native.Time_GetMonotonicTimestamp())
Native.Sleep(1000)
print(Native.Time_GetMonotonicTimestamp())