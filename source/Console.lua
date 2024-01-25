local PATH = "C:/Users/Silve/Develop/Projects/CSOL24H/"
if (not Console)
then
    Console = {}
    -- 在控制台上打印字符串，不换行
    -- @param str
    -- @return nil
    function Console:print(str)
        OutputLogMessage(str)
    end

    -- 在控制台上打印字符串，并换行
    -- @param str
    -- @return nil
    function Console:println(str)
        OutputLogMessage(str .. '\n')
    end

    -- 在控制台上按照固定格式打印信息
    -- @param str
    -- @return nil
    function Console:infomation(str)
        self:println("[INFO] " .. str)
    end

    -- 在控制台上按照固定格式打印错误信息
    -- @param str
    -- @return nil
    function Console:error(str)
        self:println("[ERR] " .. str)
    end

    -- 清空控制台
    -- @param nil
    -- @return nil
    function Console:clear()
        ClearLog()
    end

end
