if not __Event_LUA__ then
    __Event_LUA__ = true
    local __version__ = "1.5.7"

    Event = {}

    Include("core/Version.lua")
    Version:set("Event", __version__)

    Event.BUILTIN = {
        PROFILE_ACTIVATED = "PROFILE_ACTIVATED",
        PROFILE_DEACTIVATED = "PROFILE_DEACTIVATED",
        G_PRESSED = "G_PRESSED",
        G_RELEASED = "G_RELEASED",
        M_PRESSED = "M_PRESSED",
        M_RELEASED = "M_RELEASED",
        MOUSE_BUTTON_PRESSED = "MOUSE_BUTTON_PRESSED",
        MOUSE_BUTTON_RELEASED = "MOUSE_BUTTON_RELEASED"
    }

    ---@type table<string, (function|nil)[]>
    Event.callback = {}

    ---注册事件。
    ---@param identifier string 事件标识符
    ---@param callback function 回调函数
    ---@return integer # 回调函数的索引
    function Event:register(identifier, callback)
        if not self.callback[identifier] then
            self.callback[identifier] = {}
        end
        table.insert(self.callback[identifier], callback)
        return #self.callback[identifier]
    end

    ---注销事件。
    ---@param identifier string 事件标识符
    ---@param id integer 回调函数的索引
    function Event:unregister(identifier, id)
        if self.callback[identifier] then
            if self.callback[identifier][id] then
                self.callback[identifier][id] = nil
            end
        end
    end

    ---触发事件。
    ---@param identifier string 事件标识符
    ---@param args table 传递给回调函数的参数
    function Event:trigger(identifier, args)
        if self.callback[identifier] then
            for _, callback in ipairs(self.callback[identifier]) do
                if callback then
                    callback(args)
                end
            end
        end
    end

end -- __Event_LUA__
