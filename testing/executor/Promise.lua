local Promise = require("executor.core.Promise")
local lu = require("luaunit")

function TestAwaitInNonAsyncEnc()
    local async_fn = Promise.async(function ()
        print("hello, world")
    end)
    lu.assertError(function() async_fn():await() end)
end

function TestAwaitInNonAsyncEnc()
    local main_coroutine = coroutine.running()
    local parent_coroutine
    local child_coroutine
    local async_parent = Promise.async(function ()
        parent_coroutine = coroutine.running()
        local async_child = Promise.async(function ()
            child_coroutine = coroutine.running()
        end)
        async_child():await()
    end)
    async_parent()
    print("main", main_coroutine)
    print("parent", parent_coroutine)
    print("child", child_coroutine)
    lu.assertNotEquals(main_coroutine, parent_coroutine)
    lu.assertNotEquals(parent_coroutine, child_coroutine)
    lu.assertNotEquals(main_coroutine, child_coroutine)
end

os.exit(lu.LuaUnit.run())

