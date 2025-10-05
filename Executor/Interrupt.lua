if not __INTERRUPT_LUA__ then
    __INTERRUPT_LUA__ = true
    local __version__ = "1.5.2"

    Include("Exception.lua")
    Include("Version.lua")
    Version:set("Interrupt", __version__)

    ---@class Interrupt
    ---@field private name string|nil 中断名称
    ---@field private handler function 中断处理函数
    ---@field private parameters table 中断处理函数参数列表
    ---@field private maskable boolean 是否可屏蔽
    ---@field private alive boolean 是否存活
    Interrupt = {}

    ---@class InterruptInitializer
    ---@field name? string
    ---@field handler function
    ---@field parameters? table
    ---@field maskable? boolean
    ---@field alive? boolean

    Interrupt.name = "__ANONYMOUS_INTERRUPT__"
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

    ---判断中断是否能够运行。
    ---@return boolean
    function Interrupt:is_alive()
        return self.alive
    end

    ---判断中断是否可屏蔽。
    ---@return boolean
    function Interrupt:is_maskable()
        return self.maskable
    end

    ---终止中断。终止后，此中断将不再被执行。
    function Interrupt:kill()
        self.alive = false
    end

    ---执行中断处理函数，并返回结果。
    ---@return ...
    function Interrupt:handle()
        if not self.alive then
            return
        end
        self.result = table.pack(self.handler(self.parameters))
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
end -- __INTERRUPT_LUA__
