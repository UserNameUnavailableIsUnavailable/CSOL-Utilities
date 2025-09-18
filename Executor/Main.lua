if not Main_lua then
    Main_lua = true
    Include("Automation.lua")
    Include("Version.lua")
    assert(
        _VERSION >= "Lua 5.4",
        ([[当前 Lua 环境版本为 %s，执行器需要 Lua 5.4 及以上版本，请确保使用最新版本的 Logitech G Hub。]]):format(
            _VERSION
        )
    )
    Version:set("Main", "1.5.3")
    Version:assert()
    ---注册完所有中断处理函数后，开中断。
    Runtime:enable_interrupt() -- 开中断

    local function interpret()
        local cmd = Command:fetch() -- 领取任务
        local task = Automation.Task[cmd]
        if (task) then task() end -- 执行任务
        Command:finish() -- 任务执行完毕
        Runtime:sleep(100)
    end

    function Main()
        while Runtime:runnable() do
            local ok, err_msg = pcall(interpret)
            if not ok then
                Error:catch(err_msg --[[@as string]])
            end
        end
    end
end -- Main_lua
