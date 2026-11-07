local Promise = require("executor.core.Promise")
local lu = require("luaunit")

function TestAwaitInNonAsyncEnc()
    local async_fn = Promise.async(function ()
        print("hello, world")
    end)
    lu.assertError(function() async_fn():await() end)
end

os.exit(lu.LuaUnit.run())

