if (not Utility_lua)
then
Utility_lua = true
Utility = {}

---封装了math.random函数，每次调用时以运行时间为随机种子产生随机数
---@param ... any 参见 `math.random`
---@see math.random
function Utility:random(...)
    math.randomseed(Runtime:get_running_time())
    return math.random(...)
end

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
---@param from integer 计数起始
---@param to integer 计数终止
---@return function # 计数器对象
function Utility:create_counter(from, to)
    from = from or 0
    to = to or 114514
    local counter = from
    return function ()
        local ret = counter
        counter = counter + 1
        if (counter > to)
        then
            counter = from
        end
        return ret
    end
end

end -- Utility_lua
