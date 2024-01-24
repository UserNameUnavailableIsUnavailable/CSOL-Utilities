local PATH = "C:/Users/Silve/Develop/Projects/CSOL24H/source/"
dofile(PATH .. "Console.lua")
dofile(PATH .. "Delay.lua")

if (not Runtime)
then
    Runtime = {}
    -- 运行时退出位，若置为true，则所有挂起、IO操作都将被跳过
    Runtime.exit = false
    -- 中断标志位，用于屏蔽中断以禁用中断嵌套
    Runtime.interuptFlag = false
    -- 获取当前程序运行时间，单位为毫秒
    -- @param nil
    -- @return nil
    function Runtime:execTime()
        return GetRunningTime()
    end

    -- 挂起当前线程，挂起后，可以处理中断事件
    -- @param duration 挂起的时间，不保证挂起的时间是否精确，尤其是有中断事件需要处理时
    -- @remark 除了Runtime内部方法外，其他地方都应当调用Runtime:sleep，而非直接调用罗技API中的Sleep，这样可以进行中断处理
    function Runtime:sleep(duration)
        -- 罗技API不支持真正的中断，故而当某个过程主动将自己挂起时（即调用Runtime:sleep)视为自发中断，此时可以处理外部事件
        if (self.exit) -- 下达退出命令后，挂起操作不被允许，直接返回
        then
            return
        end
        -- 中断处理
        self:interuptHandler()
        Sleep(duration)
    end

    -- 中断处理函数
    -- @param nil
    -- @return nil
    -- @remark 只有中断标志位失能时才允许中断，允许中断时，按下右Ctrl、右Alt、右Shift触发中断
    function Runtime:interuptHandler()
        if (self.interuptFlag) -- 未关中断才会触发中断
        then
            return
        end
        -- 右alt、右ctrl、右shift同时按下触发中断
        if (not (Keyboard:modifierPressed(Keyboard.RALT) and Keyboard:modifierPressed(Keyboard.RCTRL) and Keyboard:modifierPressed(Keyboard.RSHIFT))) 
        then
            return
        end
        Console:infomation("Interupt: Begin handling interupt.")
        -- 中断开始时，中断标志位使能以屏蔽中断
        self.interuptFlag = true
        -- 中断现场（临时变量，自动回收，以免后患）
        local interuptContext = {
                keyboardUnreleased = {},
                mouseUnreleased = {}
            }
        -- 现场切换，保护中断现场
        self:protectContext(interuptContext)
        while (true)
        do
            Sleep(Delay.LONG)
            -- 锁定值为0（全不亮），左Ctrl、左Alt、左Shift按下（表示确认），则退出位使能，此时不会恢复中断现场，任何后续的挂起、外围设备操作将直接返回，资源随之释放
            if (Keyboard:getKeyLockState() == 0 and
            (Keyboard:modifierPressed(Keyboard.LALT) and Keyboard:modifierPressed(Keyboard.LCTRL) and Keyboard:modifierPressed(Keyboard.LSHIFT)))
            then
                Runtime.exit = true
                Console:infomation("Interupt: Runtime:exit bit is set.")
                break
            -- 锁定值为7（全亮），左Ctrl、左Alt、左Shift按下（表示确认），则撤销中断处理并恢复中断现场，程序执行不受到影响
            elseif (Keyboard:getKeyLockState() ~= 0 and
            (Keyboard:modifierPressed(Keyboard.LALT) and Keyboard:modifierPressed(Keyboard.LCTRL) and Keyboard:modifierPressed(Keyboard.LSHIFT))) 
            then
                -- 恢复现场
                self:restoreContext(interuptContext)
                Console:infomation("Interupt: Cancel interupt operations.")
                break
            end
            -- 中断处理完毕，中断标志位失能
        end
        self.interuptFlag = false
        Console:infomation("Interupt: Finish Handling interupt.")
    end

    -- 保护中断现场：把所有按下但未释放的键盘按键和鼠标按钮全部弹起，避免干扰中断处理，随后可以进行上下文切换
    -- @param (table)interuptContext 将中断现场保存到表中
    -- @return nil
    function Runtime:protectContext(interuptContext)
        for key, _ in pairs(Keyboard.unreleased)
        do
            Keyboard:release(key)
            interuptContext.keyboardUnreleased[key] = true
        end
        for button, _ in pairs (Mouse.unreleased)
        do
            Mouse:release(button)
            interuptContext.mouseUnreleased[button] = true
        end
    end

    -- 恢复中断现场：中断处理完成后，重新按下保存中断现场中的按下但未弹起的键盘按键和鼠标按钮
    -- @param (table)interuptContext 中断现场
    -- @return nil
    function Runtime:restoreContext(interuptContext)
        for key, _ in pairs(interuptContext.keyboardUnreleased)
        do
            Keyboard:press(key)
            interuptContext.keyboardUnreleased[key] = nil
        end
        for button, _ in pairs(interuptContext.mouseUnreleased)
        do
            Mouse:press(button)
            interuptContext.mouseUnreleased[button] = nil
        end
    end

    -- 阻塞等待某个流程执行完毕
    -- @param (function)procedure 需要等待的流程
    -- @param ... procedure接收的参数
    -- @return 与procedure相同
    function Runtime:waitFor(procedure, ...)
        if (type(procedure) == "function")
        then
            return procedure(...)
        end
    end

end