if not Console_lua then
    Console_lua = true

    Include("Emulator.lua")
    Include("Version.lua")
    Version:set("Console", "1.5.4")
    Console = {}

    ---在控制台上打印字符串，不换行。
    ---@param fmt string
    ---@param ... any
    function Console:printf(fmt, ...)
        OutputLogMessage(fmt, ...)
    end

    ---在控制台上打印字符串，不换行。
    ---@param ... any
    function Console:print(...)
        local str = ""
        local len = select("#", ...)
        for i, v in ipairs({ ... }) do
            if type(v) ~= "string" then
                str = str .. tostring(v)
            else
                str = str .. v
            end
            if i < len then
                str = str .. " "
            end
        end
        self:printf("%s", str)
    end

    ---日志等级。
    ---@enum Console.LOG_LEVELS
    Console.LOG_LEVELS = {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        ERROR = 4,
    }

    Console.log_level = Console.LOG_LEVELS.INFO

    ---设置日志等级，低于该等级的日志将不会被打印。
    ---@param level Console.LOG_LEVELS
    function Console:set_log_level(level)
        self.log_level = level
    end

    ---获取当前日志等级。
    ---@return Console.LOG_LEVELS
    function Console:get_log_level()
        return self.log_level
    end

    ---在控制台上按照固定格式打印跟踪日志。
    ---@param ... any
    function Console:trace(...)
        if self:get_log_level() > self.LOG_LEVELS.TRACE then
            return
        end
        local args = { ... }
        args[#args+1] = "\n"
        self:print("【跟踪】", table.unpack(args))
    end

    ---在控制台上按照固定格式打印调试日志。
    ---@param ... any
    function Console:debug(...)
        if self:get_log_level() > self.LOG_LEVELS.DEBUG then
            return
        end
        local args = { ... }
        args[#args+1] = "\n"
        self:print("【调试】", table.unpack(args))
    end

    ---在控制台上按照固定格式打印信息日志。
    ---@param ... any
    function Console:info(...)
        if self:get_log_level() > self.LOG_LEVELS.INFO then
            return
        end
        local args = { ... }
        args[#args+1] = "\n"
        self:print("【信息】", table.unpack(args))
    end

    ---在控制台上按照固定格式打印警告日志。
    ---@param ... any
    function Console:warning(...)
        if self:get_log_level() > self.LOG_LEVELS.WARNING then
            return
        end
        local args = { ... }
        args[#args+1] = "\n"
        self:print("【警告】", table.unpack(args))
    end

    ---在控制台上按照固定格式打印错误日志。
    ---@param ... any
    function Console:error(...)
        if self:get_log_level() > self.LOG_LEVELS.ERROR then
            return
        end
        local args = { ... }
        args[#args+1] = "\n"
        self:print("【错误】", table.unpack(args))
    end

    ---清空控制台。
    function Console:clear()
        ClearLog()
    end
end -- Console_lua
