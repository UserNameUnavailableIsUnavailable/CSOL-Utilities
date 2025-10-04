---该文件定义了罗技 G HUB 的其中部分函数，仅用于建立测试环境（需要在本机上提供 Lua 5.4 运行环境）。
---当在 G HUB 中运行时，不会运行此脚本。
if not Emulator_lua then
    Emulator_lua = true
    Include("Version.lua")
    Version:set("Emulator", "1.5.2")

    RunningInEmulator = false -- 在 G HUB 中运行

    if
        not require -- 没有提供 require，说明在 G HUB 中运行
    then
        return
    end
    assert(_VERSION == "Lua 5.4", "该脚本需要 Lua 5.4 运行环境。")
    assert(os.getenv("OS") == "Windows_NT", "该脚本仅支持 Windows 系统。")
    assert(os.getenv("PROCESSOR_ARCHITECTURE") == "AMD64", "仅支持 Intel IA-32e 及 AMD64 架构处理器。")
    package.cpath = package.cpath .. ";" .. PATH:format("?.dll")
    local LGHUB_Emulator = require("LGHUB_Emulator")

    RunningInEmulator = true -- 本地运行

    LGHUB_Emulator.InitializeRunningTime()

    Include("Constants.lua")
    ---休眠指定的时间。
    ---@param milliseconds number 休眠的时间（单位：毫秒）
    function Sleep(milliseconds)
        LGHUB_Emulator.Sleep(milliseconds)
    end

    ---输出日志信息。
    ---@param fmt string 日志格式化字符串
    ---@param ... any 日志格式化参数
    function OutputLogMessage(fmt, ...)
        io.write(fmt:format(...))
    end

    ---获取脚本运行时间。
    ---@return number # 脚本运行时间（单位：毫秒）
    function GetRunningTime()
        return LGHUB_Emulator.GetRunningTime()
    end

    ---获取日期/时间，与 os.date 函数相同。
    ---@param format string | nil
    ---@param time integer | nil
    ---@return string | osdate # 日期/时间
    function GetDate(format, time)
        return os.date(format, time)
    end

    function ClearLog()
        LGHUB_Emulator.ClearLog()
    end

    ---检查指定的键盘锁定键是否开启。
    ---@param key string 键盘锁定键名称（"capslock"、"numlock"、"scrolllock"）
    ---@return boolean
    function IsKeyLockOn(key)
        if key == "capslock" then
            key = "CapsLock"
        elseif key == "numlock" then
            key = "NumLock"
        elseif key == "scrolllock" then
            key = "Scroll"
        else
            error("不支持的键盘锁定键。")
        end
        local handle = io.popen(
            ([[powershell -NoProfile -Command "Add-Type -AssemblyName System.Windows.Forms; [System.Windows.Forms.Control]::IsKeyLocked([System.Windows.Forms.Keys]::%s)"]]):format(
                key
            )
        )
        assert(handle, "与外部 powershell 交互失败。")
        local result = handle:read("*a")
        handle:close()
        result = result:gsub("^%s+", ""):gsub("%s+$", "")
        if result == "True" then
            return true
        else
            return false
        end
    end

    local function execute_keyboard_operation(op, key)
        if type(key) == "number" then
            op(key)
        elseif type(key) == "string" then
            local hit = false
            for k, v in pairs(Constants.keyboard_names) do
                if v == key then
                    op(Constants.keyboard_scancodes[k])
                    hit = true
                    break
                end
            end
            if not hit then
                error(("Invalid key name: %s."):format(key))
            end
        end
    end

    ---按下指定的键。
    ---@param ... integer | string 按键名称或扫描码
    function PressKey(...)
        for i = 1, select("#", ...) do
            local key = select(i, ...)
            execute_keyboard_operation(LGHUB_Emulator.PressKey, key)
        end
    end
    ---释放指定的键。
    ---@param ... integer | string 按键名称或扫描码
    function ReleaseKey(...)
        for i = 1, select("#", ...) do
            local key = select(i, ...)
            execute_keyboard_operation(LGHUB_Emulator.ReleaseKey, key)
        end
    end
    ---按下并释放指定的键。
    ---@param ... integer | string 按键名称或扫描码
    function PressAndReleaseKey(...)
        for i = 1, select("#", ...) do
            local key = select(i, ...)
            execute_keyboard_operation(LGHUB_Emulator.PressAndReleaseKey, key)
        end
    end
    ---按下指定的鼠标按钮。
    ---@param button integer 鼠标按钮序号（1, 2, 3, 4, 5）
    function PressMouseButton(button)
        LGHUB_Emulator.PressMosueButton(button)
    end
    ---释放指定的鼠标按钮。
    ---@param button integer 鼠标按钮序号（1, 2, 3, 4, 5）
    function ReleaseMouseButton(button)
        LGHUB_Emulator.ReleaseMouseButton(button)
    end
    ---按下并释放指定的鼠标按钮。
    ---@param button integer 鼠标按钮序号（1, 2, 3, 4, 5）
    function PressAndReleaseMouseButton(button)
        LGHUB_Emulator.PressAndReleaseMouseButton(button)
    end
    ---移动鼠标光标到指定位置。
    ---@param x integer 鼠标光标的横坐标
    ---@param y integer 鼠标光标的纵坐标
    function MoveMouseTo(x, y)
        LGHUB_Emulator.SetMousePosition(x, y)
    end
    ---移动鼠标光标到虚拟屏幕中的指定位置。
    ---@param x integer 鼠标光标的横坐标
    ---@param y integer 鼠标光标的纵坐标
    function MoveMouseToVirtual(x, y)
        LGHUB_Emulator.SetMousePosition(x, y)
    end
    ---相对当前位置移动鼠标光标。
    ---@param downward number 鼠标光标向下移动的距离（负数表示向上）
    ---@param rightward number 鼠标光标向右移动的距离（负数表示向左）
    function MoveMouseRelative(downward, rightward)
        LGHUB_Emulator.MoveMouseRelative(downward, rightward)
    end

    ---获取鼠标光标的当前坐标。
    ---@return integer x, integer y 鼠标光标的坐标
    function GetMousePosition()
        return LGHUB_Emulator.GetMousePosition()
    end

    ---判断指定的鼠标按钮是否被按下。
    ---@param button integer 鼠标按钮序号（1, 2, 3, 4, 5）
    ---@return boolean # 是否被按下
    function IsMouseButtonPressed(button)
        return false
    end
    ---向 Windows Debugger 输出调试信息。
    ---@param fmt string 调试信息格式化字符串
    ---@param ... any 调试信息格式化参数
    function OutputDebugMessage(fmt, ...)
        LGHUB_Emulator.OutputDebugMessage(fmt:format(...))
    end
    ---滚动鼠标滚轮。
    ---@param count number 滚动的次数（正数表示向上滚动，负数表示向下滚动）
    function MoveMouseWheel(count)
        LGHUB_Emulator.MoveMouseWheel(count)
    end
    ---判断修饰键是否按下。
    ---@param key string 按键名称
    ---@return boolean
    function IsModifierPressed(key)
        local state = LGHUB_Emulator.GetModifierState()
        if key == "alt" then
            return (state & (0x3 << 0)) ~= 0
        elseif key == "lalt" then
            return (state & (0x1 << 0)) ~= 0
        elseif key == "ralt" then
            return (state & (0x1 << 1)) ~= 0
        elseif key == "shift" then
            return (state & (0x3 << 2)) ~= 0
        elseif key == "lshift" then
            return (state & (0x1 << 2)) ~= 0
        elseif key == "rshift" then
            return (state & (0x1 << 3)) ~= 0
        elseif key == "ctrl" then
            return (state & (0x3 << 4)) ~= 0
        elseif key == "lctrl" then
            return (state & (0x1 << 4)) ~= 0
        elseif key == "rctrl" then
            return (state & (0x1 << 5)) ~= 0
        end

        error(
            [[invalid argument (expected one of 'alt', 'lalt', 'ralt', 'shift', 'lshift', 'rshift', 'ctrl', 'lctrl' or 'rctrl')]]
        )
    end

    function IsEmulating()
        return LGHUB_Emulator.IsEmulating()
    end
end
