local PATH = "C:/Users/Silve/Develop/CSOL-24H/"
if (not Utility)
then
    Utility = {}
    -- 封装了math.random函数，每次调用时以运行时间为随机种子产生随机数
    -- @param 同math.random
    -- @remark 参见math.random了解参数信息
    function Utility:generateRandom(...)
        math.randomseed(Runtime:get_running_time())
        return math.random(...)
    end

    -- 返回一个（循环）计数器
    -- @param [from] 计数起始
    -- @param [to]   计数终止
    -- @remark 如果未提供from参数，则计数器从0开始递增计数；to为循环计数器上限，达到上限后计数器回到from（wrap-around），若不提供to参数则不设上限
    function Utility:counter(from, to)
        from = from or 0
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

    -- 获取日期
    -- @param 同os.date
    -- @remark 参见os.date了解参数信息
    function Utility:date(...)
        return GetDate(...)
    end

    end
