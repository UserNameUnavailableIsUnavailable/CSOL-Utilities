if (not Console_lua)
then
    Console_lua = true
    Include("Version.lua")
    Version:set("Console", { 1, 5, 1 })
    Console = {}
    ---在控制台上打印字符串，不换行
    ---@param fmt string
    ---@param ... any
    ---@return nil
    function Console:printf(fmt, ...)
        OutputLogMessage(fmt, ...)
    end

    ---在控制台上按照固定格式打印信息
    ---@param fmt string 
    ---@param ... any
    ---@return nil
    function Console:information(fmt, ...)
        self:printf("【信息】" .. fmt .. '\n', ...)
    end

    ---在控制台上按照固定格式打印警告
    ---@param fmt string 
    ---@param ... any
    ---@return nil
    function Console:warning(fmt, ...)
        self:printf("【警告】" .. fmt .. '\n', ...)
    end

    ---在控制台上按照固定格式打印错误
    ---@param fmt string
    ---@return nil
    function Console:error(fmt, ...)
        self:printf("【错误】" .. fmt .. '\n', ...)
    end

    ---清空控制台
    ---@return nil
    function Console:clear()
        ClearLog()
    end

end -- Console_lua
