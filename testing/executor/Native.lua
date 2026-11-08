local Native = require("executor.core.Native")
local lu = require("luaunit")

function TestGetMonotonicTimestamp()
    local t = Native.GetMonotonicTimestamp()
    lu.assertTrue(t >= 0)
end

function TestGetSystemTimestamp()
    local t1 = Native.GetSystemTimestamp() / 1000
    local t2 = os.time()
    lu.assertAlmostEquals(t1, t2, 2500)
end

os.exit(lu.LuaUnit.run())