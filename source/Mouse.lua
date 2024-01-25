local PATH = "C:/Users/Silve/Develop/Projects/CSOL24H/"
dofile(PATH .. "source/Runtime.lua")
dofile(PATH .. "source/Delay.lua")
----- 鼠标按钮设定 -----
if (not Mouse)
then
    Mouse = {
        LEFT = 1, MIDDLE = 2 ,RIGHT = 3, X1 = 4, X2 = 5
    }

    -- 鼠标按下但未释放的按钮，true表示按下，nil表示弹起
    Mouse.unreleased = {}

    -- 按下某个按钮，但不释放
    -- @param button 按钮值，如Mouse.LEFT
    -- @param [delay] 按下某个按钮后的延迟时间
    -- @return nil
    -- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
    function Mouse:press(button, delay)
        if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
        then
            return
        end
        PressMouseButton(button)
        self.unreleased[button] = true
        if (type(delay) == "number")
        then
            Runtime:sleep(delay)
        end
    end

    -- 释放某个按钮
    -- @param button 按钮值，如Mouse.LEFT
    -- @param [delay] 释放某个按钮后的延迟时间
    -- @return nil
    -- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
    function Mouse:release(button, delay)
        if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
        then
            return
        end
        ReleaseMouseButton(button)
        self.unreleased[button] = nil
        if (type(delay) == "number")
        then
            Runtime:sleep(delay)
        end
    end

    -- 释放所有通过Mouse:press按下但未回弹的按钮（记录在Mouse.unreleased中）
    -- @param [delay] 释放每个按钮后的延迟时间
    -- @return nil
    -- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
    function Mouse:releaseAll(delay)
        if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
        then
            return
        end
        for button, _ in pairs(self.unreleased)
        do
            ReleaseMouseButton(button)
            self.unreleased[button] = nil
            Runtime:sleep(delay)
        end
    end

    -- 单击一次按钮
    -- @param button 按钮值，如Mouse.LEFT
    -- @param [delay] 点击某个按钮后的延迟时间
    -- @return nil
    -- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
    function Mouse:click(button, delay)
        if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
        then
            return
        end
        PressAndReleaseMouseButton(button)
        if (type(delay) == "number")
        then
            Runtime:sleep(delay)
        end
    end

    -- 双击时间间隔150ms（人手实际双击间隔时间一般为此值）
    Mouse.DOUBLE_CLICK_INTERVAL = Delay.MEDIUM 

    -- 双击一次按钮
    -- @param button 按钮值，如Mouse.LEFT
    -- @param [delay] 双击某个按钮后的延迟时间
    -- @return nil
    -- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
    function Mouse:doubleClick(button, delay)
        if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
        then
            return
        end
        PressAndReleaseMouseButton(button)
        Sleep(self.DOUBLE_CLICK_INTERVAL)
        PressAndReleaseMouseButton(button)
        if (type(delay) == "number")
        then
            Runtime:sleep(delay)
        end
    end

    -- 使用鼠标单击屏幕上某个位置
    -- @param x 横坐标
    -- @param y 纵坐标
    -- @return nil
    -- @remark 坐标(x, y)获取方式参见Mouse:locate 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
    function Mouse:clickOn(x, y)
        if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
        then
            return
        end
        Mouse:moveTo(x, y, Delay.SHORT)
        Mouse:click(Mouse.LEFT, Delay.SHORT)
    end

    -- 使用鼠标双击屏幕上某个位置
    -- @param x 横坐标
    -- @param y 纵坐标
    -- @return nil
    -- @remark 坐标(x, y)获取方式参见Mouse:locate 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
    function Mouse:doubleClickOn(x, y)
        if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
        then
            return
        end
        Mouse:moveTo(x, y, Delay.SHORT)
        Mouse:doubleClick(Mouse.LEFT, Delay.SHORT)
    end

    -- 判断某个按钮是否按下
    -- @param button 按钮值，如Mouse.LEFT
    -- @return nil
    function Mouse:pressed(button)
        return IsMouseButtonPressed(button)
    end

    -- 获取鼠标光标位置
    -- @param nil
    -- @return x, y 对应于横、纵坐标
    function Mouse:locate()
        return GetMousePosition()
    end

    -- 移动鼠标光标到某位置
    -- @param x 横坐标
    -- @param y 纵坐标
    -- @param [delay] 移动鼠标光标后的延迟时间
    -- @return nil
    -- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
    function Mouse:moveTo(x, y, delay)
        if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
        then
            return
        end
        MoveMouseTo(x, y)
        if (type(delay) == "number")
        then
            Runtime:sleep(delay)
        end
    end

    -- 相对移动鼠标光标
    -- @param rightward 向右移动的偏移量，负数表示向左
    -- @param downward 向下移动的偏移量，负数表示向上
    -- @param [delay] 移动光标后的延迟
    -- @return nil
    -- @remark 当Runtime.exit为true时，该函数将直接返回，不进行任何操作
    function Mouse:moveRelative(rightward, downward, delay)
        if (Runtime.exit) -- 当下达退出指令时，不进行任何操作
        then
            return
        end
        MoveMouseRelative(math.ceil(rightward), math.ceil(downward))
        if (type(delay) == "number")
        then
            Runtime:sleep(delay)
        end
    end

end
