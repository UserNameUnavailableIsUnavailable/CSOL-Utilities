if (not Runtime_lua)
then
Include("Context.lua")
Runtime_lua = true
Runtime = {}
---手动接管标志。必须通过用户手动恢复。接管期间，所有键鼠操作将被跳过。
Runtime.manual_flag = false
---暂停标志。若置为 `true`，则所有键鼠操作都将被跳过。
-- Runtime.pause_flag = false
---中断标志位，用于开/关中断，避免中断嵌套。`true` 允许中断，`false` 不允许中断。
Runtime.interrupt_flag = false
---中断发生时保存的中断现场。
---@type Context[]
Runtime.interrupt_context = {}
---获取当前程序运行时间，单位为毫秒。
---@return integer
function Runtime:get_running_time()
    return GetRunningTime()
end

---期望的睡眠时间
Runtime.expected_sleep_time = 10
---实际睡眠时间粒度
Runtime.actual_sleep_time = 10

Runtime.interrupt_flag_stack = {}

---将中断标志位压栈。
function Runtime:push_interrupt_flag()
    self.interrupt_flag_stack[#self.interrupt_flag_stack + 1] = self.interrupt_flag_stack
end

---从栈中恢复最近一次的中断标志位。
function Runtime:pop_interrupt_flag()
    if (#self.interrupt_flag_stack < 1)
    then
        return
    end
    self.interrupt_flag = self.interrupt_flag_stack[#self.interrupt_flag_stack]
    self.interrupt_flag_stack[#self.interrupt_flag_stack] = nil
end

---开中断。
function Runtime:sti()
    Runtime.interrupt_flag = true
end

---关中断。
function Runtime:cli()
    Runtime.interrupt_flag = false
end

---挂起当前执行流，挂起后，可以处理中断事件。除了 `Runtime` 内部方法外，其他地方都应当调用 `Runtime:sleep`，而非直接调用罗技 API 中的 Sleep，这样可以进行中断处理。
---@param milliseconds integer | nil 挂起的时间。
---@param precise boolean | nil 是否需要尽力保证精度。
---@return nil
function Runtime:sleep(milliseconds, precise)
    milliseconds = milliseconds or 0
    milliseconds = math.floor(milliseconds) -- 防止提供小数时间，若类型非 `number` 会返回 `nil`
    -- 将长时间的休眠拆分为若干短时间休眠，确保 `Runtime` 常常保持对程序的控制权
    while (true)
    do
        local expected_sleep_time = 0.5 * self.expected_sleep_time + 0.5 * self.actual_sleep_time
        -- 罗技 API 不支持真正的中断，故而当某个过程主动将自己挂起时（即调用Runtime:sleep)视为自发中断，此时可以处理外部事件
        local before_int = Runtime:get_running_time()
        -- 先执行中断处理
        self:interrupt_handler()
        -- 中断处理结束后，再校验参数（无论如何都要进行中断处理，即便参数非法）
        if (type(milliseconds) ~= "number" or milliseconds < 0)
        then
            return
        end
        -- 预测下一次休眠会消耗的时间
        local after_int = Runtime:get_running_time()
        local int_time = after_int - before_int
        milliseconds = milliseconds - int_time -- 去除中断处理耗时

        if (milliseconds > expected_sleep_time) -- 大于预计休眠时间
        then
            local start_timepoint = Runtime:get_running_time()
            Sleep(10) -- 按照 10 ms 时间片大小进行休眠
            local end_timepoint = Runtime:get_running_time()
            local real_sleep_time = end_timepoint - start_timepoint
            milliseconds = milliseconds - real_sleep_time -- 减去实际睡眠时间
            -- 更新预测值和实际值
            self.expected_sleep_time, self.actual_sleep_time = expected_sleep_time, real_sleep_time
        elseif (milliseconds > 0) -- 0 ＜ milliseconds ≤ expected_sleep_time
        then
            if (precise) -- 需要较高精度，剩余时间采用忙等
            then
                local begin = Runtime:get_running_time()
                repeat
                until Runtime:get_running_time() - begin >= milliseconds -- 忙等以确保精度符合要求
            else
                Sleep(milliseconds) -- 不考虑精度，按照剩余时间直接休眠
            end
            break
        else -- milliseconds ≤ 0
            break
        end
    end
end

---中断处理函数列表。
Runtime.interrupt_handlers = {}

---默认的中断处理函数（不进行任何操作）。只有中断标志位使能时才允许中断。
function Runtime:interrupt_handler()
    if (not Runtime.interrupt_flag) -- 未关中断才会触发中断
    then
        return
    end
    local int_status, int_error
    -- 中断开始时，中断标志位使能以屏蔽后续中断
    self:push_interrupt_flag()
    self:cli() -- 关中断，避免在中断处理过程中再次触发中断，导致中断嵌套
    for i = 1, #self.interrupt_handlers
    do
        -- 执行中断处理，若处理过程中出现错误，则先暂存错误，目的是确保 `interrupt_flag` 正常恢复
        if (self.interrupt_handlers[i])
        then
            int_status, int_error = pcall(self.interrupt_handlers[i])
        end
    end
    -- 中断处理完毕
    self:pop_interrupt_flag() -- 开中断
    if (not int_status) -- 中断处理出现错误
    then
        error(int_error:__tostring()) -- 将中断处理过程中引发的错误上抛
    end
end

---注册中断处理函数。若 `f` 类型非 `function`，则返回值为 0。
---@param f function 中断处理函数。
---@return integer index 中断处理函数索引。
function Runtime:register_interrupt_handler(f)
    if (type(f) ~= "function")
    then
        return 0
    end
    local index = #self.interrupt_handlers + 1
    self.interrupt_handlers[index] = f
    return index
end

---注销中断处理函数。
---@param index integer
---@return boolean success 操作是否成功
function Runtime:unregister_interrput_handler(index)
    if (self.interrupt_handlers[index])
    then
        self.interrupt_handlers[index] = nil
        return true
    end
    return false
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