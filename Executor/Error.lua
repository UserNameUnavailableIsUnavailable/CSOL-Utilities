if not Error_lua then
    Error_lua = true
    Include("JSON.lua")
    Include("Version.lua")
    Version:set("Error", "1.5.4")

    ---@class Error
    ---@field name string 错误名称
    ---@field message string 错误消息
    ---@field parameters table 错误参数列表，此参数列表会被传递给错误处理函数
    Error = {}

    Error.error_handlers = {}
    Error.HEADER = "EXECUTOR_ERROR_OBJECT"
    Error.name = "SUCCESS"
    Error.message = ""
    Error.parameters = {}
    Error.__index = Error
    setmetatable(Error, Error)

    ---获取最近一次通过 `throw` 抛出的错误。
    ---@return table
    function Error:get_last_error()
        return {
            name = self.name,
            message = self.message,
            parameters = self.parameters,
        }
    end

    ---设置最近一次通过 `throw` 抛出的错误。
    ---@param e table
    ---@return boolean
    function Error:set_last_error(e)
        if e.name ~= "string" or e.message ~= "string" or e.parameters ~= table then
            return false
        else
            self.name = e.name
            self.message = e.message
            self.parameters = e.parameters
            return true
        end
    end

    ---注册错误处理函数。
    ---@param name string 异常处理函数名称
    ---@param handler function 异常处理函数
    function Error:register_error_handler(name, handler)
        if type(handler) == "function" then
            Error.error_handlers[name] = handler
        end
    end

    ---注销错误处理函数。
    ---@param error_name string 错误处理函数名称
    function Error:unregister_error_handler(error_name)
        Error.error_handlers[error_name] = nil
    end

    ---抛出异常。
    ---@param init table
    function Error:throw(init)
        local name = init.name or "UNDEFINED_ERROR_NAME"
        local message = init.message or "UNDEFINED_ERROR_MESSAGE"
        local parameters = {}
        init.parameters = init.parameters or {}
        if type(init.parameters) ~= "table" then
            parameters[1] = tostring(init.parameters)
        else
            for i, v in ipairs(init.parameters) do
                if type(v) == "nil" or type(v) == "number" or type(v) == "boolean" or type(v) == "string" then
                    parameters[i] = v
                end
            end
        end
        local error_table = {
            type = self.HEADER,
            name = name,
            message = message,
            parameters = parameters,
        }
        local error_json_string = JSON.encode(error_table)

        -- Error 中保存最近一次错误信息
        self.name = name
        self.message = message
        self.parameters = parameters

        error(error_json_string)
    end

    ---错误处理函数，调用者不应该捕获 catch 抛出的错误。
    ---@param error_string string 由 pcall / xpcall 返回的字符串化错误信息
    function Error:catch(error_string)
        local ok, result
        local error_init, error_detail
        local error_json_string

        if type(error_string) ~= "string" then
            error_init = {
                name = "INVALID_ERROR_TYPE",
                message = ("传入的错误信息必须为 `string`，但接收到的类型为 `%s`。"):format(
                    type(error_string)
                ),
            }
            goto FATAL
        end

        ok, result = pcall(JSON.decode, error_json_string)

        if not ok then
            error_init = {
                name = "INVALID_ERROR_JSON_FORMAT",
                message = "未知错误信息格式（无法解析 JSON 格式的错误信息）。",
                parameters = { error_json_string, result },
            }
            goto FATAL
        end

        -- 检查是否存在对应的处理函数
        if not self.error_handlers[result.name] then
            error_init = {
                name = "ERROR_HANDLER_NOT_FOUND",
                message = ("找不到错误名称 `%s` 对应的错误处理函数"):format(result.name),
                parameters = { result.name },
            }
            goto FATAL
        end

        -- 将对象原型设置为 `self`
        result.__index = self
        setmetatable(result, self)
        ok, result = pcall(self.error_handlers[result.name], table.unpack(self.parameters))
        if not ok then
            if type(result) == "table" then
                if
                    type(result.name) == "string"
                    and type(result.message) == "string" -- result 满足可抛出条件
                then
                    Error:throw({
                        name = result.name,
                        message = result.message,
                        parameters = result.parameters,
                    })
                end
            end
            error_init = {
                name = "ERROR_HANDLER_FAILED",
                message = "执行错误处理函数发生错误",
                parameters = { tostring(result) },
            }
            goto FATAL
        end

        -- 错误处理完毕
        goto OK
        ::FATAL::
        -- 错误处理过程中又出现了新的错误
        Error:fatal()
        Error:throw(error_init)
        ::OK::
        return
    end

    Error.fatal_error_disposals = {}

    ---注册发生灾难错误后的处理函数，一般用于回收一些无法自行释放的资源（如按下的按键）。
    ---@param f function 处理函数
    ---@return integer index 从 `1` 开始编号的处理函数的索引，若 `f` 不是函数，则返回 `0`。
    function Error:register_fatal_disposal(f)
        local index = #self.fatal_error_disposals + 1
        if type(f) == "function" then
            self.fatal_error_disposals[index] = f
            return index
        end
        return 0
    end

    ---注销发生灾难错误够的处理函数。
    ---@param index integer 索引号
    ---@return boolean status 操作是否成功完成
    function Error:unregister_fatal_disposal(index)
        if Error.fatal_error_disposals[index] then
            Error.fatal_error_disposals[index] = nil
            return true
        end
        return false
    end

    ---出现灾难错误后的处理。
    function Error:fatal()
        for i = 1, #self.fatal_error_disposals do
            local disposal = Error.fatal_error_disposals[i]
            if disposal then
                pcall(disposal)
            end
        end
    end
end -- Error_lua
