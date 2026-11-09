local Native = require("executor.Native")
local Logitech = require("executor.Logitech")

print(Native.GetMonotonicTimepoint())
Native.Sleep(1000)
print(Native.GetMonotonicTimepoint())