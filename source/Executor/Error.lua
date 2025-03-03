if not Error_lua
then
    Error_lua = true
    ---@class Error
    ---@field type string 错误种类
    ---@field message string 错误消息
    ---@field parameters table 错误参数列表，此参数列表会被传递给错误处理函数
    Error = {}

    Error.error_handlers = {}
    Error.MAGIC = "EXECUTOR_ERROR_OBJECT"
    Error.type = "SUCCESS"
    Error.message = ""
    Error.parameters = {}
    Error.__index = Error
    setmetatable(Error, Error)

    ---注册错误处理函数。
    ---@param name string 异常处理函数名称
    ---@param handler function 异常处理函数
    function Error:register_error_handler(name, handler)
        if (type(handler) == "function")
        then
            Error.error_handlers[name] = handler
        end
    end

    ---注销错误处理函数。
    ---@param error_type string 错误处理函数名称
    function Error:unregister_error_handler(error_type)
        Error.error_handlers[error_type] = nil
    end

    ---抛出异常。
    ---@param init table
    function Error:throw(init)
        self.type = init.type or "UNDEFINED"
        self.message = init.message or "UNDEFINED_ERROR_MESSAGE"
        self.parameters = init.parameters or {}
        error(string.format("%s: ❌%s❌", self.MAGIC, tostring(self)))
    end

    ---错误处理函数，调用者不应该捕获 catch 抛出的错误。
    ---@param error_string string 由 pcall / xpcall 返回的字符串化错误信息
    function Error:catch(error_string)

        -- 解析错误，错误初始化列表由两个 ❌ 包裹，采用正则表达式解析
        local snippet = "return " .. error_string:match("❌(.-)❌")
        local error_init, error_detail
        local initializer
        local ok, result

        if (not snippet)
        then
            error_init = {
                type = "ERROR_CATCH_FAILED",
                message = "无效的错误信息格式",
                parameters = { error_string }
            }
            goto FATAL
        end
        
        -- 获取初始化器
        initializer, error_detail = load(snippet)
        if (not initializer)
        then
            error_init = {
                type = "ERROR_CATCH_FAILED",
                message = "无效的错误初始化器",
                parameters = { ("function () %s end"):format(snippet), error_detail }
            }
            goto FATAL
        end

        -- 调用初始化器，初始化错误对象
        ok, result = xpcall(
            initializer --[[@as function]],
            function ()
                error_detail = debug.traceback()
            end
        )
        if (not ok)
        then
            error_init = {
                type = "ERROR_CATCH_FAILED",
                message = "调用初始化器初始化错误对象失败",
                parameters = { error_detail }
            }
            goto FATAL
        end

        -- 检查是否存在对应的处理函数
        if (not self.error_handlers[result.type])
        then
            error_init = {
                type = "ERROR_CATCH_ERROR_HANDLER_NOT_FOUND",
                message = "找不到此类型对应的错误处理函数",
                parameters = { result.type }
            }
            goto FATAL
        end

        -- 将对象原型设置为 `self`
        result.__index = self
        setmetatable(result, self)
        ok = xpcall(
            self.error_handlers[result.type],
            function ()
                error_detail = debug.traceback()
            end,
            table.unpack(self.parameters)
        )
        if (not ok)
        then
            error_init = {
                type = "ERROR_CATCH_ERROR_HANDLER_ERROR",
                message = "在调用错误处理函数进行错误处理时出现新的错误",
                parameters = { result.type, error_detail }
            }
            goto FATAL
        end

        -- 错误处理完毕
        self.type = "SUCCESS"
        self.message = ""
        self.parameters = {}
        goto OK
        ::FATAL::
        -- 错误处理过程中又出现了新的错误
        Error:fatal()
        Error:throw(error_init) -- 抛出该错误将终止整个程序运行
        ::OK::
        return
    end

    function Error:__tostring()
        local parameters = ""
        for i, v in ipairs(self.parameters)
        do
            if (type(v) == "string")
            then
                parameters = parameters .. '\"' .. v .. '\"'
            else
                parameters = parameters .. tostring(v)
            end
            if (i ~= #self.parameters)
            then
                parameters = parameters .. ", "
            end
        end
        return string.format("{ type = \"%s\", message = \"%s\", parameters = { %s } }", self.type, self.message, parameters)
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