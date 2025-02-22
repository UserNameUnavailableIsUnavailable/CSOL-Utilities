if (not Runtime_lua)
then
Include("Context.lua")
Runtime_lua = true
Runtime = {}
---手动接管标志。若置为 `true` 则 `pause_flag` 被屏蔽，且无法由程序自身恢复，必须通过用户手动恢复。接管期间，所有键鼠操作将被跳过。
Runtime.manual_flag = false
---暂停标志。若置为 `true`，则所有键鼠操作都将被跳过。
-- Runtime.pause_flag = false
---中断标志位，用于开/关中断，避免中断嵌套。
Runtime.interrupt_flag = true
---中断发生时保存的中断现场。
---@type Context[]
Runtime.interrupt_context = {}
---获取当前程序运行时间，单位为毫秒。
---@return integer
function Runtime:get_running_time()
    return GetRunningTime()
end

---挂起当前用户级线程，挂起后，可以处理中断事件。除了 `Runtime` 内部方法外，其他地方都应当调用 `Runtime:sleep`，而非直接调用罗技 API 中的 Sleep，这样可以进行中断处理。
---@param milliseconds integer | nil 挂起的时间，不保证挂起的时间是否精确，尤其是在有中断事件需要处理时。
---@return nil
function Runtime:sleep(milliseconds)
    -- 罗技API不支持真正的中断，故而当某个过程主动将自己挂起时（即调用Runtime:sleep)视为自发中断，此时可以处理外部事件
    self:interrupt_handler()
    if (type(milliseconds) ~= "number" or milliseconds < 1)
    then
        return
    end
    milliseconds = math.floor(milliseconds)
    if (milliseconds <= 10)
    then
        Sleep(milliseconds)
    else
        -- 将长时间的休眠拆分为若干短时间休眠，确保 `Runtime` 常常保持对程序的控制权
        local quotient = milliseconds / 10
        local remainder = milliseconds % 10
    while (quotient > 0)
    do
        Runtime:sleep(10)
        quotient = quotient - 1
    end
        if (remainder > 0)
        then
            Sleep(remainder)
        end
    end
end

---中断处理函数列表。
Runtime.interrupt_handler_list = {}

---默认的中断处理函数（不进行任何操作）。只有中断标志位使能时才允许中断。
function Runtime:interrupt_handler()
    if (not Runtime.interrupt_flag) -- 未关中断才会触发中断
    then
        return
    end
    -- 中断开始时，中断标志位使能以屏蔽后续中断
    self.interrupt_flag = false -- 关中断
    for i = 1, #self.interrupt_handler_list
    do
        self.interrupt_handler_list[i]()
    end
    -- 中断处理完毕
    self.interrupt_flag = true -- 开中断
end

---注册中断处理函数。若 `f` 类型非 `function`，则返回值为 0。
---@param f function 中断处理函数。
---@return integer index 中断处理函数索引。
function Runtime:register_interrupt_handler(f)
    if (type(f) ~= "function")
    then
        return 0
    end
    local index = #self.interrupt_handler_list + 1
    self.interrupt_handler_list[index] = f
    return index
end

---注销中断处理函数。
---@param index integer
function Runtime:unregister_interrput_handler(index)
    table.remove(self.interrupt_handler_list, index)
end

---注册中断现场，中断发生时保存。
---@param context_object Context
---@return boolean
function Runtime:register_context(context_object)
    if (type(context_object.save_callback) == "function" and type(context_object.restore_callback) == "function")
    then
        self.interrupt_context[#self.interrupt_context + 1] = context_object
        return true
    end
    return false
end

---注销中断现场。
---@param context_object Context
---@return boolean
function Runtime:unregister_context(context_object)
    for index, value in pairs(self.interrupt_context)
    do
        if (value == context_object)
        then
            table.remove(self.interrupt_context, index)
            return true
        end
    end
    return false
end
---保护中断现场。把所有按下但未释放的键盘按键和鼠标按钮全部弹起，避免干扰中断处理，随后可以进行上下文切换。
---@return nil
function Runtime:save_context()
    for _, obj in pairs(self.interrupt_context)
    do
        obj:save_callback()
    end
end

---恢复中断现场：中断处理完成后，重新按下保存中断现场中的按下但未弹起的键盘按键和鼠标按钮
---@return nil
function Runtime:restore_context()
    for _, obj in pairs(self.interrupt_context)
    do
        obj:restore_callback()
    end
end

---判断是否处于停止状态，停止状态下跳过所有键鼠操作。
function Runtime:is_paused()
    return self.manual_flag
end

end -- Runtime_lua