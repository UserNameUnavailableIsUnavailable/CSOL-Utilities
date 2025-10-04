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
    end

    function Main()
        while Runtime:runnable() do
            Runtime:try_catch_finally(
                function ()
                    interpret()
                    Runtime:sleep(100) -- sleep 会抛出错误，因此不可放到 finally 中
                end,
                ---处理异常
                ---@param e Exception
                function (e)
                    Console:debug(([[捕获到异常：%s]]):format(tostring(e)))
                    if (not Automation:is_ignored_error(e.name)) then
                        Runtime:fatal()
                    end
                end
            )
        end
    end
end -- Main_lua
