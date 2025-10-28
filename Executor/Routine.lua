if not __ROUTINE_LUA__ then
    __ROUTINE_LUA__ = true
    local __version__ = "1.5.6"

    Include("Exception.lua")
    Include("Version.lua")
    Version:set("Routine", __version__)

    ---@class Routine
    ---@field private name string|nil 例程名称
    ---@field private handler function 例程处理函数
    ---@field private parameters table 例程处理函数参数列表
    ---@field private maskable boolean 是否可屏蔽
    ---@field private alive boolean 是否存活
    Routine = {}

    ---@class RoutineInitializer
    ---@field handler function
    ---@field name? string
    ---@field parameters? table
    ---@field maskable? boolean
    ---@field alive? boolean

    Routine.name = "ANONYMOUS"
    Routine.handler = function(...) end
    Routine.parameters = {}
    Routine.maskable = true
    Routine.alive = true
    Routine.result = nil

    ---创建例程。
    ---@param init function|RoutineInitializer 例程处理函数或初始化列表
    ---@return Routine
    function Routine:new(init)
        local routine = {}
        if type(init) == "function" then
            routine.handler = init
        else -- table
            -- 只接受下面的字段，其他字段忽略
            routine = {
                name = init.name,
                handler = init.handler,
                parameters = init.parameters,
                maskable = init.maskable,
                alive = init.alive,
            }
        end
        self.__index = self
        setmetatable(routine, self)
        return routine --[[@as Routine]]
    end

    ---判断例程是否能够运行。
    ---@return boolean
    function Routine:is_alive()
        return self.alive
    end

    ---判断例程是否可屏蔽。
    ---@return boolean
    function Routine:is_maskable()
        return self.maskable
    end

    ---终止例程。终止后，此例程将不再被执行。
    function Routine:kill()
        self.alive = false
    end

    ---执行例程处理函数，并返回结果。
    ---@return ...
    function Routine:handle()
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

    function Routine:get_result()
        return self.result
    end
end -- __ROUTINE_LUA__
