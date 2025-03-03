if not Error_lua
then
    Error_lua = true
    ---@class Error
    ---@field type string 错误类型
    ---@field parameters table 错误参数列表，此参数列表会被传递给错误处理函数
    Error = {}
    Error.handlers = {}

    Error.type = "SUCCESS"
    Error.parameters = {}

    ---注册错误处理函数。
    ---@param name string 异常处理函数名称
    ---@param handler function 异常处理函数
    function Error:register_error_handler(name, handler)
        if (type(handler) == "function")
        then
            Error.handlers[name] = handler
        end
    end

    ---注销错误处理函数。
    ---@param error_type string 错误处理函数名称
    function Error:unregister_error_handler(error_type)
        Error.handlers[error_type] = nil
    end

    ---抛出异常。
    ---@param err Error 错误对象
    function Error:throw(err)
        error(err)
    end

    ---错误处理函数，调用者不应该捕获 catch 抛出的错误。
    ---@param err any
    function Error:catch(err)
        local status, another_err
        another_err = Error:new({type="FATAL"})
        if (type(err) ~= "table")
        then
            status = false
            if (type(err) == "string")
            then
                Console:error(err)
            end
            Console:error("非法的错误对象：%s。", type(err))
        elseif (not self.handlers[err.type])
        then
            status = false
            Console:error("无法处理的错误类型：%s。", err.type)
        else
            status, another_err = xpcall(
                self.handlers[err.type],
                function ()
                    Console:println(debug.traceback())
                end,
                table.unpack(err.parameters)
            )
        end
        if (not status)
        then
            Error:fatal()
            Error:throw(another_err) -- 抛出该错误将终止整个程序运行
        end
    end

    ---构造错误对象。
    ---@param init table
    function Error:new(init)
        local error = init or {}
        self.__index = self
        setmetatable(error, self)
        return error
    end

    Error.fatal_error_disposals = {}

    ---注册发生灾难错误后的处理函数，一般用于回收一些无法自行释放的资源（如按下的按键）。
    ---@param f function 处理函数
    ---@return integer index 从 `1` 开始编号的处理函数的索引，若 `f` 不是函数，则返回 `0`。
    function Error:register_fatal_disposal(f)
        local index = #self.fatal_error_disposals + 1
        if (type(f) == "function")
        then
            self.fatal_error_disposals[index] = f
            return index
        end
        return 0
    end

    ---注销发生灾难错误够的处理函数。
    ---@param index integer 索引号
    ---@return boolean status 操作是否成功完成
    function Error:unregister_fatal_disposal(index)
        if (Error.fatal_error_disposals[index])
        then
            Error.fatal_error_disposals[index] = nil
            return true
        end
        return false
    end

    ---出现灾难错误后的处理。
    function Error:fatal()
        for i = 1, #self.fatal_error_disposals
        do
            local disposal = Error.fatal_error_disposals[i]
            if (disposal)
            then
                pcall(disposal)
            end
        end
    end

end -- Error_lua