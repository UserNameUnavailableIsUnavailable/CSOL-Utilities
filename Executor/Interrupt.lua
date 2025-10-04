if not Interrupt_lua then
    Interrupt_lua = true

    Include("Exception.lua")
    Include("Version.lua")
    Version:set("Interrupt", "1.5.2")

    ---@class Interrupt
    ---@field name string|nil 中断名称
    ---@field handler function 中断处理函数
    ---@field parameters any[]|nil 中断处理函数参数列表
    ---@field maskable boolean|nil 是否可屏蔽
    ---@field alive boolean|nil 是否存活
    Interrupt = {}

    ---@class InterruptInitializer
    ---@field name? string
    ---@field handler function
    ---@field parameters? any[]
    ---@field maskable? boolean
    ---@field alive? boolean

    Interrupt.name = "<ANONYMOUS_INTERRUPT>"
    Interrupt.handler = function(...) end
    Interrupt.parameters = {}
    Interrupt.maskable = true
    Interrupt.alive = true
    Interrupt.result = nil

    ---创建中断。
    ---@param init function|InterruptInitializer 中断处理函数或初始化列表
    ---@return Interrupt
    function Interrupt:new(init)
        local interrupt = {}
        if type(init) == "function" then
            interrupt.handler = init
        else -- table
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
        return interrupt --[[@as Interrupt]]
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
