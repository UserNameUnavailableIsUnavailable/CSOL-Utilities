if not __ERROR_LUA__ then
    __ERROR_LUA__ = true
    local __version__ = "1.5.4"
    
    Include("JSON.lua")
    Include("Version.lua")
    Version:set("Error", __version__)

    ---@class Exception
    ---@field private name string 异常名称
    ---@field private message string 异常消息
    ---@field private supplement table 补充信息
    ---@field private traceback? string 异常堆栈
    Exception = {
        __MAGIC__ = "1145141919810",
        name = "__ERROR_DEFAULT__",
        message = "Default Error.",
        supplement = {},
        traceback = nil,
    }

    ---@class ExceptionInitializer
    ---@field name string
    ---@field message string
    ---@field supplement? table

    ---创建一个异常对象。
    ---@param init ExceptionInitializer
    ---@return Exception
    ---@nodiscard
    function Exception:new(init)
        local obj = {
            name = init.name,
            message = init.message,
            supplement = init.supplement
        }
        setmetatable(obj, self)
        self.__index = self
        return obj --[[@as Exception]]
    end

    ---判断一个对象是否为异常对象。
    ---@param e any
    ---@return boolean
    function Exception:is_instance(e)
        if type(e) ~= "table" then
            return false
        end
        return self.__MAGIC__ == e.__MAGIC__
    end

    ---设置异常堆栈，一经设置便无法更改。
    ---@param traceback string
    ---@return boolean
    function Exception:set_traceback(traceback)
        if not self.traceback then
            self.traceback = traceback
            return true
        end
        return false
    end

    ---获取异常堆栈。
    ---@return string|nil
    function Exception:get_traceback()
        return self.traceback
    end

    ---获取异常名称。
    ---@return string
    ---@nodiscard
    function Exception:get_name()
        return self.name
    end

    ---获取异常消息。
    ---@return string
    function Exception:get_message()
        return self.message
    end

    ---获取异常补充信息。
    ---@return table
    ---@nodiscard
    function Exception:get_supplement()
        local supplement = {}
        -- 通过元表实现补充信息的只读，这样可以防止外部修改补充信息。
        supplement.__index = self.supplement
        setmetatable(supplement, self.supplement)
        return supplement
    end

    function Exception:__tostring()
        return JSON.encode({
            name = self.name,
            message = self.message,
            supplement = self.supplement,
            traceback = self.traceback
        })
    end
end -- __ERROR_LUA__
