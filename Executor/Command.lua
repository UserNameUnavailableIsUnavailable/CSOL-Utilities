if not __COMMAND_LUA__ then
    __COMMAND_LUA__ = true
    local __version__ = "1.5.3"

    Include("Version.lua")

    Version:set("Command", __version__)

    Command = {
        CMD_NOP = 0,
        CMD_START_GAME_ROOM = 1,
        CMD_CHOOSE_CHARACTER = 2,
        CMD_DEFAULT_IDLE = 3, -- 执行挂机动作，并进行结算确认
        CMD_EXTENDED_IDLE = 4, -- 执行挂机动作，并进行结算确认
        CMD_CONFIRM_RESULTS = 5,
        CMD_CREATE_GAME_ROOM = 6,
        CMD_BATCH_COMBINE_PARTS = 7,
        CMD_BATCH_PURCHASE_ITEM = 8,
        CMD_LOCATE_CURSOR = 9,
        CMD_CLEAR_POPUPS = 10,
        CMD_DETECT_IN_GAME = 11,
        CMD_DEFAULT_IDLE_2 = 12, -- 与 CMD_DEFAULT_IDLE 功能相同，但不会进行结算确认
        CMD_EXTENDED_IDLE_2 = 13, -- 与 CMD_EXTENDED_IDLE 功能相同，但不会进行结算确认
    }

    Command.IDLE_COMMANDS = {
        Command.CMD_DEFAULT_IDLE,
        Command.CMD_EXTENDED_IDLE,
        Command.CMD_DEFAULT_IDLE_2,
        Command.CMD_EXTENDED_IDLE_2,
    }

    ---判断一个命令是否为挂机命令。
    ---@param cmd any
    function Command:is_idle_command(cmd)
        for _, v in ipairs(Command.IDLE_COMMANDS) do
            if v == cmd then
                return true
            end
        end
        return false
    end

    Command.status = 0
    Command.UNCHANGED = 0 -- 与上次相比，命令未发生更新
    Command.IDENTIFIER_CHANGED = 0x1 -- 与上次相比，命令标识符发生变化
    Command.TYPE_CHANGED = 0x2 -- 与上次相比，命令的类型发生变化
    Command.TIMEPOINT_CHANGED = 0x4 -- 与上次相比，命令的时间戳发生变化
    Command.REPEATABILITY_CHANGED = 0x8 -- 与上次相比，命令可重复性发生变化

    Command.id = 0
    Command.type = Command.CMD_NOP
    Command.timepoint = 0
    Command.repeatable = true
    Command.finished = false -- 命令是否执行结束，注意对于可重复执行的命令，此字段一直保持为 `false`

    function Command:get_status()
        return self.status
    end

    ---指令的默认内容。
    ---读取命令文件后，`Command.directives` 会被更新为最新的命令内容。
    Command.directives = {
        id = 0,
        type = Command.CMD_NOP,
        timepoint = 0,
        repeatable = true,
    }

    ---读取命令文件。
    function Command:receive()
        local status = 0
        pcall(Include, "Directives.lua") -- 若读取命令文件失败，则仍保留上次的命令不变
        if Command.directives.id ~= self.id then
            self.id = Command.directives.id
            self.finished = false -- 新的命令，将完成状态设置为 false
            status = status | Command.IDENTIFIER_CHANGED
        end
        if Command.directives.type ~= self.type then
            self.type = Command.directives.type
            status = status | Command.TYPE_CHANGED
        end
        if Command.directives.timepoint ~= self.timepoint then
            self.timepoint = Command.directives.timepoint
            status = status | Command.TIMEPOINT_CHANGED
        end
        if Command.directives.repeatable ~= self.repeatable then
            self.repeatable = Command.directives.repeatable
            status = status | Command.REPEATABILITY_CHANGED
        end
        Command.status = status
    end

    ---判断当前命令是否有效。
    ---@return boolean
    function Command:is_valid()
        local current_time = DateTime:get_local_timestamp() -- 本地时间戳
        local expired = current_time - self.timepoint > 5 -- 命令是否过期

        -- 命令过期
        if expired then
            return false
        end
        -- 命令已经执行结束
        if self.finished then
            return false
        end
        return true
    end

    ---标记当前命令为已完成。
    function Command:finish()
        if not self.repeatable then
            self.finished = true
        else
            self.finished = false
        end
    end

    ---领取待执行命令。
    function Command:fetch()
        if self:is_valid() then
            return self.type
        end
        return Command.CMD_NOP
    end
end -- __COMMAND_LUA__
