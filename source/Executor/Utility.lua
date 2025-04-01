if (not Utility_lua)
then
    Utility_lua = true
    Include("Runtime.lua")
    Include("Version.lua")
    Version:set("Utility", { 1, 5, 1 })
    Utility = {}

    ---等概率随机正负方向。
    ---@return integer `1` 或 `-1`
    function Utility:random_direction()
        math.randomseed(Runtime:get_running_time())
        if (math.random() < 0.5)
        then
            return 1
        else
            return -1
        end
    end
    ---返回一个（循环）计数器。如果未提供 `from` 参数，则计数器从 `0` 开始递增计数；
    ---`to`为循环计数器上限，达到上限后计数器回到 `from`，若不提供 `to` 参数则计数上限为 `114514`。
    ---@param from number 计数起始
    ---@param to number 计数终止
    ---@param step number 计数步长
    ---@return function # 计数器对象
    function Utility:create_counter(from, to, step)
        step = step or 1
        to = to or 1145141919810
        local counter = from
        return function ()
            local ret = counter
            counter = counter + step
            if (counter >= to)
            then
                counter = from
            end
            return ret
        end
    end

    ---向 Windows 调试器汇报 JSON 格式的消息。
    ---@param t table 消息内容
    function Utility:report(t)
        local json = JSON.encode(t)
        OutputDebugMessage(json)
    end

end -- Utility_lua
