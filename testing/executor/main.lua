local Native = require("executor.Native")
local Logitech = require("executor.Logitech")

print(Native.Time_GetMonotonicTimestamp())
Native.Sleep(1000)
print(Native.Time_GetMonotonicTimestamp())