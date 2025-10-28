if not __MAIN_LUA__ then
    __MAIN_LUA__ = true
    local __version__ = "1.5.7"

    Include("Automation.lua")
    Include("Command.lua")
    Include("Runtime.lua")
    Include("Version.lua")
    Include("Console.lua")
    Include("Exception.lua")

    assert(
        _VERSION >= "Lua 5.4",
        ([[当前 Lua 环境版本为 %s，执行器需要 Lua 5.4 及以上版本，请确保使用最新版本的 Logitech G Hub。]])
        :format(_VERSION)
    )

    Version:set("Main", __version__)
    Version:require("Main", "Runtime", "1.5.7")
    
    Version:assert()
    ---注册完所有例程处理函数后，开例程。
    Runtime:enable_routine() -- 开例程

    local function interpret()
        local cmd = Command:fetch() -- 领取任务
        local task = Automation.Task[cmd]
        if (task) then task() end -- 执行任务
        Command:finish() -- 任务执行完毕
    end

    function StartUp()
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
                    if (not Automation:is_ignored_error(e:get_name())) then
                        Runtime:fatal(e)
                    end
                end
            )
        end
    end

    ---程序入口。
    ---@param args? table 参数
    function Main(args)
        StartUp()
    end
end -- __MAIN_LUA__
