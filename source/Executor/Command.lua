if (not Command_lua)
then
Command_lua = true
Command = {
    CMD_NOP = 0,
    CMD_START_GAME_ROOM = 1,
    CMD_CHOOSE_CHARACTER = 2,
    CMD_DEFAULT_IDLE = 3,
    CMD_EXTENDED_IDLE = 4,
    CMD_CONFIRM_RESULTS = 5,
    CMD_CREATE_GAME_ROOM = 6,
    CMD_BATCH_COMBINE_PARTS = 7,
    CMD_BATCH_PURCHASE_ITEM = 8,
    CMD_LOCATE_CURSOR = 9,
    CMD_CLEAR_POPUPS = 10
}

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

---读取命令文件。
function Command:update()
    local status = 0
    pcall(Include, "$~cmd.lua") -- 若读取命令文件失败，则仍保留上次的命令不变
    CmdId = CmdId or 0
    CmdType = CmdType or Command.CMD_NOP
    CmdTimepoint = CmdTimepoint or 0
    if (CmdId ~= self.id)
    then
        self.id = CmdId
        self.finished = false -- 新的命令
        status = status | Command.IDENTIFIER_CHANGED
    end
    if (CmdType ~= self.type)
    then
        self.type = CmdType
        status = status | Command.TYPE_CHANGED
    end
    if (CmdTimepoint ~= self.timepoint)
    then
        self.timepoint = CmdTimepoint
        status = status | Command.TIMEPOINT_CHANGED
    end
    if (CmdRepeatable ~= self.repeatable)
    then
        self.repeatable = not self.repeatable
        status = status | Command.REPEATABILITY_CHANGED
    end
    Command.status = status
end

function Command:is_valid()
    local current_time = DateTime:get_local_timestamp() -- 本地时间戳
    local expired = current_time - self.timepoint > 5 -- 命令是否过期

    -- 命令过期
    if expired
    then
        return false
	end
    -- 命令已经执行结束
    if self.finished
    then
        return false
	end
    return true
end

function Command:finish()
    if (not self.repeatable)
    then
        self.finished = true
    else
        self.finished = false
    end
end

---领取待执行命令。
function Command:claim()
    if (self:is_valid())
    then
        return self.type
    end
    return Command.NOP
end

end -- Command_lua