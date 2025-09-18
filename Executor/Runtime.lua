if not Runtime_lua then
    Runtime_lua = true

    Include("Emulator.lua")
    Include("Context.lua")
    Include("Interrupt.lua")
    Include("JSON.lua")
    Include("Version.lua")
    Version:set("Runtime", "1.5.2")

    ---@class Runtime 运行时
    ---@field interrupts Interrupt[] 中断列表。
    ---@field interrupt_mode integer 中断模式
    ---@field INTERRUPT_BURST_MODE integer 猝发中断模式。
    ---@field INTERRUPT_SEQUENCE_MODE integer 顺序中断模式。
    ---@field INTERRUPT_RANDOM_MODE integer 随机中断模式。
    ---@field interrupt_mask_flag boolean 中断屏蔽标志位。`true` 屏蔽中断，`false` 允许受理中断。
    ---@field interrupt_busy_flag boolean 中断忙标志，用于避免中断嵌套。`true` 表示当前正在受理其他中断，`false` 表示空闲。
    ---@field interrupt_mask_flag_stack boolean[] 存放中断屏蔽标志位的栈。
    ---@field interrupt_context Context[] 中断发生时保存的中断现场。
    ---@field expected_sleep_time number 根据最近睡眠情况推测的一轮睡眠时间
    ---@field actual_sleep_time number 最近一次实际的一轮睡眠时间
    ---@field last_interrupt_id integer 最近一次处理的中断对应标识符
    Runtime = {}

    Runtime.INTERRUPT_BURST_MODE = 0
    Runtime.INTERRUPT_SEQUENCE_MODE = 1
    Runtime.INTERRUPT_RANDOM_MODE = 2
    Runtime.interrupt_mode = Runtime.INTERRUPT_BURST_MODE -- 默认采用猝发式中断

    Runtime.interrupt_mask_flag = false
    Runtime.interrupt_busy_flag = false
    Runtime.interrupt_context = {}

    ---获取当前程序运行时间，单位为毫秒。
    ---@return integer
    function Runtime:get_running_time()
        return GetRunningTime()
    end

    Runtime.expected_sleep_time = 10
    Runtime.actual_sleep_time = 10

    Runtime.interrupt_mask_flag_stack = {}

    ---将中断标志位压栈。
    function Runtime:push_interrupt_mask_flag()
        self.interrupt_mask_flag_stack[#self.interrupt_mask_flag_stack + 1] = self.interrupt_mask_flag
    end

    ---从栈中恢复最近一次的中断标志位。
    function Runtime:pop_interrupt_mask_flag()
        if #self.interrupt_mask_flag_stack < 1 then
            return
        end
        self.interrupt_mask_flag = self.interrupt_mask_flag_stack[#self.interrupt_mask_flag_stack]
        self.interrupt_mask_flag_stack[#self.interrupt_mask_flag_stack] = nil
    end

    ---开中断。
    function Runtime:enable_interrupt()
        Runtime.interrupt_mask_flag = false
    end

    ---关中断。
    function Runtime:disable_interrupt()
        Runtime.interrupt_mask_flag = true
    end

    ---判断中断是否处于屏蔽状态。
    ---@return boolean
    function Runtime:is_interrupt_masked()
        return Runtime.interrupt_mask_flag
    end

    ---挂起当前执行流，挂起后，可以处理中断事件。除了 `Runtime` 内部方法外，其他地方都应当调用 `Runtime:sleep`，而非直接调用罗技 API 中的 Sleep，这样可以进行中断处理。
    ---@param milliseconds integer | nil 挂起的时间。
    ---@param precise boolean | nil 是否需要尽力保证精度。
    ---@return nil
    function Runtime:sleep(milliseconds, precise)
        milliseconds = milliseconds or 0
        milliseconds = math.floor(milliseconds) -- 防止提供小数时间，若类型非 `number` 会返回 `nil`
        -- 将长时间的休眠拆分为若干短时间休眠，确保 `Runtime` 常常保持对程序的控制权
        while true do
            local expected_sleep_time = 0.5 * self.expected_sleep_time + 0.5 * self.actual_sleep_time
            -- 罗技 API 不支持真正的中断，故而当某个过程主动将自己挂起时（即调用Runtime:sleep)视为自发中断，此时可以处理外部事件
            local before_int = Runtime:get_running_time()
            -- 先执行中断处理
            self:interrupt()
            -- 中断处理结束后，再校验参数（无论如何都要进行中断处理，即便参数非法）
            if type(milliseconds) ~= "number" or milliseconds < 0 then
                return
            end
            -- 预测下一次休眠会消耗的时间
            local after_int = Runtime:get_running_time()
            local int_time = after_int - before_int
            milliseconds = milliseconds - int_time -- 去除中断处理耗时

            if
                milliseconds > expected_sleep_time -- 大于预计休眠时间
            then
                local start_timepoint = Runtime:get_running_time()
                Sleep(10) -- 按照 10 ms 时间片大小进行休眠
                local end_timepoint = Runtime:get_running_time()
                local real_sleep_time = end_timepoint - start_timepoint
                milliseconds = milliseconds - real_sleep_time -- 减去实际睡眠时间
                -- 更新预测值和实际值
                self.expected_sleep_time, self.actual_sleep_time = expected_sleep_time, real_sleep_time
            elseif
                milliseconds > 0 -- 0 ＜ milliseconds ≤ expected_sleep_time
            then
                if
                    precise -- 需要较高精度，剩余时间采用忙等
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

    Runtime.interrupts = {}

    ---粗发式中断处理。
    function Runtime:interrupt_in_burst()
        local int_status
        local int_result --[[@as any]]
        int_result = "INTERRUPT_HANDLER_SUCCESS"
        if
            Runtime.interrupt_busy_flag -- 当前有正在处理的中断，跳过
        then
            return
        end
        -- 中断开始时，中断标志位使能以屏蔽后续中断
        for _, interrupt in ipairs(self.interrupts) do
            -- 执行中断处理，若处理过程中出现错误，则先暂存错误，目的是确保 `interrupt_flag` 正常恢复
            if
                not interrupt:is_maskable() or not self:is_interrupt_masked() -- 该中断不可屏蔽，或处于开中断状态
            then
                self:push_interrupt_mask_flag()
                self:disable_interrupt() -- 关中断，避免在中断处理过程中再次触发中断，导致中断嵌套
                self.interrupt_busy_flag = true
                int_status, int_result = pcall(function() interrupt:handle() end) -- 处理中断
                self.interrupt_busy_flag = false
                self:pop_interrupt_mask_flag() -- 开中断
            end
            -- 当前中断处理完毕
            if
                not int_status -- 中断处理出现错误
            then
                error(int_result) -- 将中断处理过程中引发的错误上抛
            end
        end
    end

    Runtime.last_interrupt_id = 0
    ---顺序式中断处理。
    function Runtime:interrupt_in_sequence()
        local int_status
        local int_result --[[@as any]]
        int_result = "INTERRUPT_HANDLER_SUCCESS"
        if
            Runtime.interrupt_busy_flag -- 当前有正在处理的中断，跳过
        then
            return
        end
        if (self.last_interrupt_id >= #self.interrupts) then -- 上一次执行的中断是最后一个
            self.last_interrupt_id = 0
        end
        local id = self.last_interrupt_id + 1
        local interrupt = self.interrupts[id] or Interrupt -- 如果为空，则用默认的中断模板代替
        -- 中断开始时，中断标志位使能以屏蔽后续中断
        -- 执行中断处理，若处理过程中出现错误，则先暂存错误，目的是确保 `interrupt_flag` 正常恢复
        if
            not interrupt:is_maskable() or not self:is_interrupt_masked() -- 该中断不可屏蔽，或处于开中断状态
        then
            self:push_interrupt_mask_flag()
            self:disable_interrupt() -- 关中断，避免在中断处理过程中再次触发中断，导致中断嵌套
            self.interrupt_busy_flag = true
            int_status, int_result = pcall(function() interrupt:handle() end) -- 处理中断
            self.interrupt_busy_flag = false
            self:pop_interrupt_mask_flag() -- 开中断
        end
        -- 中断处理完毕，修改最近一次执行的中断 ID
        self.last_interrupt_id = id
        if
            not int_status -- 中断处理出现错误
        then
            error(int_result) -- 将中断处理过程中引发的错误上抛
        end
    end

    ---随机式中断处理。
    function Runtime:interrupt_at_random()
        local int_status
        local int_result --[[@as any]]
        int_result = "INTERRUPT_HANDLER_SUCCESS"
        if
            Runtime.interrupt_busy_flag -- 当前有正在处理的中断，跳过
        then
            return
        end
        local interrupt = Interrupt
        if #self.interrupts > 0 then
            interrupt = self.interrupts[math.random(1, #self.interrupts)]
        end
        -- 中断开始时，中断标志位使能以屏蔽后续中断
        -- 执行中断处理，若处理过程中出现错误，则先暂存错误，目的是确保 `interrupt_flag` 正常恢复
        if
            not interrupt:is_maskable() or not self:is_interrupt_masked() -- 该中断不可屏蔽，或处于开中断状态
        then
            self:push_interrupt_mask_flag()
            self:disable_interrupt() -- 关中断，避免在中断处理过程中再次触发中断，导致中断嵌套
            self.interrupt_busy_flag = true
            int_status, int_result = pcall(function() interrupt:handle() end) -- 处理中断
            self.interrupt_busy_flag = false
            self:pop_interrupt_mask_flag() -- 开中断
        end
        -- 中断处理完毕
        if
            not int_status -- 中断处理出现错误
        then
            error(int_result) -- 将中断处理过程中引发的错误上抛
        end
    end

    function Runtime:interrupt()
        if self.interrupt_mode == self.INTERRUPT_BURST_MODE then
            self:interrupt_in_burst()
        elseif self.interrupt_mode == self.INTERRUPT_SEQUENCE_MODE then
            self:interrupt_in_sequence()
        elseif self.interrupt_mode == self.INTERRUPT_RANDOM_MODE then
            self:interrupt_at_random()
        end
    end

    ---切换采用猝发式中断/顺序式中断。
    ---@param mode integer
    function Runtime:set_interrupt_mode(mode)
        self.interrupt_mode = mode
    end

    ---注册中断。
    ---@param interrupt Interrupt 中断回调函数或中断处理流程初始化列表
    ---@return integer id
    function Runtime:register_interrupt(interrupt)
        local id = #self.interrupts + 1
        self.interrupts[id] = interrupt
        return id
    end

    ---注销中断。
    ---@param id integer 中断标识符
    ---@return boolean
    function Runtime:unregister_interrput(id)
        if self.interrupts[id] then
            self.interrupts[id]:kill()
            return true
        end
        return false
    end

    ---注册中断现场，中断发生时保存。
    ---@param context_object Context
    ---@return boolean
    function Runtime:register_context(context_object)
        if type(context_object.save_callback) == "function" and type(context_object.restore_callback) == "function" then
            self.interrupt_context[#self.interrupt_context + 1] = context_object
            return true
        end
        return false
    end

    ---注销中断现场。
    ---@param context_object Context
    ---@return boolean
    function Runtime:unregister_context(context_object)
        for index, value in pairs(self.interrupt_context) do
            if value == context_object then
                table.remove(self.interrupt_context, index)
                return true
            end
        end
        return false
    end

    ---保护中断现场。把所有按下但未释放的键盘按键和鼠标按钮全部弹起，避免干扰中断处理，随后可以进行上下文切换。
    ---@return nil
    function Runtime:save_context()
        for _, obj in pairs(self.interrupt_context) do
            obj:save_callback()
        end
    end

    ---恢复中断现场：中断处理完成后，重新按下保存中断现场中的按下但未弹起的键盘按键和鼠标按钮
    function Runtime:restore_context()
        for _, obj in pairs(self.interrupt_context) do
            obj:restore_callback()
        end
    end

    ---判断是否在模拟环境中运行。
    ---@return boolean
    function Runtime:emulating()
        return RunningInEmulator
    end

    ---判定运行状态，若在 LGHUB 中运行，则返回 true；若在模拟器中运行，则只有在模拟状态下才返回 true。
    ---@return boolean
    function Runtime:runnable()
        return not RunningInEmulator or IsEmulating()
    end
end -- Runtime_lua
