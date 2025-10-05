if not __CONTEXT_LUA__ then
    __CONTEXT_LUA__ = true

    Include("Version.lua")
    Version:set("Context", "1.5.2")
    ---@class Context
    Context = {}

    Context.save_callback = nil
    Context.restore_callback = nil
    ---@type any[]
    Context.storage = nil

    ---构造一类中断上下文对象。
    ---@param save_callback function 保存中断现场的回调函数。
    ---@param restore_callback function 恢复中断现场的回调函数。
    ---@return Context
    function Context:new(save_callback, restore_callback)
        local obj = {}
        self.__index = self
        obj.save_callback = save_callback
        obj.restore_callback = restore_callback
        obj.storage = {}
        setmetatable(obj, self)
        return obj
    end
end -- __CONTEXT_LUA__
