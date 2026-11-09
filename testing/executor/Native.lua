local Native = require("executor.Native")
local lu = require("luaunit")

function TestGetMonotonicTimepoint()
    local t = Native.GetMonotonicTimepoint()
    lu.assertTrue(t >= 0)
end

function TestGetSystemTimepoint()
    local t1 = Native.GetSystemTimepoint() / 1000
    local t2 = os.time()
    lu.assertAlmostEquals(t1, t2, 2500)
end

os.exit(lu.LuaUnit.run())