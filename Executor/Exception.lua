if not Error_lua then
    Error_lua = true
    Include("JSON.lua")
    Include("Version.lua")
    Version:set("Error", "1.5.4")

    ---@class Exception
    ---@field name string 错误名称
    ---@field message string 错误消息
    ---@field supplement table 补充信息
    Exception = {
        name = "__DefaultError__",
        message = "Default Error.",
        supplement = {},
    }

    ---@class ExceptionInitializer
    ---@field name string
    ---@field message string
    ---@field supplement? table

    ---创建一个错误对象。
    ---@param init ExceptionInitializer
    ---@return Exception
    ---@nodiscard
    function Exception:new(init)
        local obj = init
        setmetatable(obj, self)
        self.__index = self
        return obj --[[@as Exception]]
    end

    ---获取错误消息。
    ---@return string
    function Exception:what()
        return self.message
    end

    function Exception:__tostring()
        return JSON.encode({
            name = self.name,
            message = self.message,
            supplement = self.supplement,
        })
    end
end -- Error_lua
