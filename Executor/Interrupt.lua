if not Interrupt_lua then
    Interrupt_lua = true

    Include("Error.lua")
    Include("Version.lua")
    Version:set("Interrupt", "1.5.2")

    ---@class Interrupt
    Interrupt = {}

    Interrupt.name = "ANONYMOUS_INTERRUPT"

    Interrupt.handler = function(...) end

    Interrupt.parameters = {}

    Interrupt.maskable = true

    Interrupt.alive = true

    Interrupt.result = nil

    ---创建中断。
    ---@param init function | table 中断处理函数或初始化列表
    ---@return Interrupt
    function Interrupt:new(init)
        local interrupt = {}
        if type(init) ~= "table" and type(init) ~= "function" then
            Error:throw({
                name = "INVALID_INTERRUPT_INIT_TYPE",
                message = "无效的中断对象初始化列表",
                parameters = { type(init) },
            })
        end
        if type(init) == "function" then
            interrupt.handler = init
        else -- table
            if type(init.name) ~= "string" and type(init.name) ~= "nil" then
                Error:throw({
                    name = "INVALID_INTERRUPT_NAME_TYPE",
                    message = ("无效的名称类型："):format(type(init.name)),
                })
            end
            if type(init.handler) ~= "function" and type(init.handler) ~= "nil" then
                Error:throw({
                    name = "INVALID_INTERRUPT_HANDLER_TYPE",
                    message = ("无效的处理函数类型："):format(type(init.handler)),
                })
            end
            if type(init.parameters) ~= "table" and type(init.parameters) ~= "nil" then
                Error:throw({
                    name = "INVALID_INTERRUPT_PARAMETERS_TYPE",
                    message = ("无效的参数列表类型："):format(type(init.parameters)),
                })
            end
            -- 只接受下面的字段，其他字段忽略
            interrupt = {
                name = init.name,
                handler = init.handler,
                parameters = init.parameters,
                maskable = init.maskable,
                alive = init.alive,
            }
        end
        self.__index = self
        setmetatable(interrupt, self)
        return interrupt
    end

    function Interrupt:is_alive()
        return self.alive
    end

    function Interrupt:is_maskable()
        return self.maskable
    end

    function Interrupt:kill()
        self.alive = false
    end

    function Interrupt:handle()
        if not self.alive then
            return
        end
        self.result = table.pack(self.handler(table.unpack(self.parameters)))
        if
            #self.result <= 1 -- 无返回值或单返回值
        then
            self.result = self.result[1]
            return self.result
        else -- 多个返回值
            return table.unpack(self.result)
        end
    end

    function Interrupt:get_result()
        return self.result
    end
end -- Interrupt_lua
