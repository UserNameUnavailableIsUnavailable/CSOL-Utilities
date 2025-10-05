if not __WEAPON_LUA__ then
    __WEAPON_LUA__ = true
    local __version__ = "1.5.4"

    Include("Runtime.lua")
    Include("Delay.lua")
    Include("Console.lua")
    Include("Mouse.lua")
    Include("Keyboard.lua")
    Include("Version.lua")
    Include("Exception.lua")
    Version:set("Weapon", __version__)
    Version:require("Weapon", "Setting", __version__, nil)

    ---@class WeaponInitializer
    ---@field public name? string
    ---@field public number? string
    ---@field public attack_button? integer
    ---@field public switch_delay? integer
    ---@field public purchase_sequence? table
    ---@field public reloading_required? boolean
    ---@field public reload_key? KEYBOARD_KEY
    ---@field public attack_duration? integer
    ---@field public horizontal_strafe_mode? string
    ---@field public vertical_strafe_mode? string
    ---@field public fire_interator? fun(self: Weapon, round: integer, begin_timepoint: integer): (function|nil)
    ---@field public strafe_interator? fun(self: Weapon, round: integer, begin_timepoint: integer): (function|nil)
    ---@field public attack? fun(self: Weapon)

    ---@class Weapon 武器类
    ---@field NULL string 配件武器
    ---@field PRIMARY string 主武器
    ---@field SECONDARY string 副武器
    ---@field MELEE string 近战武器
    ---@field GRENADE string 手雷
    ---@field protected name string 武器名
    ---@field protected number string 武器栏位
    ---@field protected attack_button MOUSE_BUTTON 攻击按钮
    ---@field protected switch_delay integer 切换延迟
    ---@field protected purchase_sequence table 购买按键序列
    ---@field protected reloading_required boolean 是否需要换弹（连续两次随机到相同的需要换弹的武器时，将其丢弃并重新购买）
    ---@field protected reload_key KEYBOARD_KEY 重新装填按键
    ---@field protected attack_duration integer 攻击持续时间，单位为秒，默认为 10 秒
    ---@field protected horizontal_strafe_mode string|nil 武器水平扫射模式，`"none"` 为无扫射，`"left"` 为固定向左，`"right"` 为固定向右，`"random"` 为随机方向扫射，`"oscillating"` 为简谐扫射（左右交替扫射）。该字段缺省值为 `"random"`。
    ---@field protected vertical_strafe_mode string|nil 武器垂直扫射模式，`"none"` 为无扫射，`"up"` 为固定向上，`"down"` 为固定向下，`"random"` 为随机方向扫射，`"oscillating"` 为简谐扫射（上下交替扫射）。该字段缺省值为 `"oscillating"`。
    ---@field protected dx integer|fun():integer 每一轮水平扫射的距离
    ---@field protected dy integer|fun():integer 每一轮垂直扫射的距离
    Weapon = {}

    Weapon.NULL = Keyboard.ZERO
    Weapon.PRIMARY = Keyboard.ONE
    Weapon.SECONDARY = Keyboard.TWO
    Weapon.MELEE = Keyboard.THREE
    Weapon.GRENADE = Keyboard.FOUR
    Weapon.name = "未知"
    Weapon.number = Weapon.NULL
    Weapon.attack_button = Mouse.LEFT
    Weapon.switch_delay = 150
    Weapon.purchase_sequence = {}
    Weapon.reloading_required = true
    Weapon.reload_key = Keyboard.R
    Weapon.attack_duration = 10
    Weapon.horizontal_strafe_mode = "random"
    Weapon.vertical_strafe_mode = "oscillating"
    Weapon.dx = 0
    Weapon.dy = 0

    ---判断武器是否需要换弹。
    ---@return boolean
    ---@nodiscard
    function Weapon:is_reloading_required()
        return self.reloading_required
    end

    ---获取武器名称。
    ---@return string
    ---@nodiscard
    function Weapon:get_name()
        return self.name
    end

    ---获取武器栏位。
    ---@return string
    ---@nodiscard
    function Weapon:get_number()
        return self.number
    end

    ---获取攻击按钮。
    ---@return MOUSE_BUTTON
    ---@nodiscard
    function Weapon:get_attack_button()
        return self.attack_button
    end

    ---获取武器切换延迟。
    ---@return integer
    ---@nodiscard
    function Weapon:get_switch_delay()
        return self.switch_delay
    end

    ---获取武器一轮攻击时长。
    ---@return integer
    ---@nodiscard
    function Weapon:get_attack_duration()
        return self.attack_duration
    end

    ---获取武器水平扫射模式。
    ---@return string
    ---@nodiscard
    function Weapon:get_horizontal_strafe_mode()
        return self.horizontal_strafe_mode
    end

    ---获取武器垂直扫射模式。
    ---@return string
    ---@nodiscard
    function Weapon:get_vertical_strafe_mode()
        return self.vertical_strafe_mode
    end

    ---获取武器购买按键序列。
    ---@return KEYBOARD_KEY[]
    ---@nodiscard
    function Weapon:get_purchase_sequence()
        local ret = {}
        -- 通过元表实现购买按键序列的只读，这样可以防止外部修改购买按键序列。
        ret.__index = self.purchase_sequence
        setmetatable(ret, self.purchase_sequence)
        return ret
    end

    ---设置换弹按键。
    ---@param key Constants.KEYBOARD_SCANCODE|Constants.KEYBOARD_NAME
    function Weapon:set_reload_key(key)
        self.reload_key = key
    end

    ---获取换弹按键。
    ---@return KEYBOARD_KEY
    function Weapon:get_reload_key()
        return self.reload_key
    end

    ---创建一个武器对象。
    ---@param init WeaponInitializer 初始化列表
    ---@return Weapon weapon 武器对象
    function Weapon:new(init)
        local weapon = init
        -- 基武器的 `__index` 指向自身
        self.__index = self
        -- 将模板武器 `weapon` 的元表设置为 `self`
        -- 这样，访问新武器中的缺省成员时，会调用模板武器的 `__index` 找到缺省值
        setmetatable(weapon, self)
        Console:info("新增武器/装备：" .. weapon.name)
        return weapon --[[@as Weapon]]
    end

    ---根据 `purchase_sequence` 字段中预设的按键序列购买武器。
    function Weapon:purchase()
        Runtime:push_interrupt_mask_flag()
        Runtime:disable_interrupt() -- 暂时关中断，完成原子操作
        for _, key in ipairs(self.purchase_sequence) do
            Keyboard:click(key, Delay.MEDIUM, true)
        end
        Runtime:pop_interrupt_mask_flag()
        if self.number == Weapon.GRENADE or self.number == Weapon.NULL then
            Keyboard:click(Weapon.MELEE, Delay.MEDIUM, true) -- 购买手雷后，临时切换到近战武器，防止后续鼠标点击导致使用诸如燃爆等武器。
        end
        if not Mouse:is_cursor_position_locked() then
            -- 清除当前界面上的所有窗口，防止购买资金不足或关闭死亡购买界面。
            Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI, Delay.SHORT)
            Mouse:click_on(
                Mouse.LEFT,
                Setting.POSITION_GAME_ESC_MENU_CANCEL_X,
                Setting.POSITION_GAME_ESC_MENU_CANCEL_Y,
                20
            ) -- 点击ESC菜单的取消按钮。
        end
    end

    ---切换到指定武器。
    function Weapon:switch()
        Keyboard:click(self.number, self.switch_delay, true)
    end

    ---切换到指定武器，不考虑切枪延迟。
    function Weapon:switch_without_delay()
        Keyboard:click(self.number, 0)
    end

    ---按下 `Keyboard.G` 键丢弃武器。
    ---@return nil
    function Weapon:abandon()
        self:switch() -- 切换到指定武器。
        Keyboard:click(Keyboard.G, Delay.LONG)
    end

    ---确认武器购买资金不足提示框（预设按钮在 Setting.lua 中）。
    ---@deprecated 此函数已经废弃，`Weapon:purchase` 会自动处理购买界面。
    function Weapon:in_case_insufficient_funds()
        Mouse:click_on(Mouse.LEFT, Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_X, Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_Y)
        Keyboard:click(Keyboard.ZERO, Delay.SHORT)
    end

    ---关闭死亡状态下的预购买菜单（点击“重复购买”按钮，不点击“取消购买”以避免与大厅界面按钮冲突）。
    ---@deprecated 此函数已经废弃，`Weapon:purchase` 会自动处理购买界面。
    function Weapon:close_dead_purchase_menu()
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_GAME_PURCHASE_BEFORE_RESPAWN_X,
            Setting.POSITION_GAME_PURCHASE_BEFORE_RESPAWN_Y,
            Delay.NORMAL
        )
    end

    ---使用特殊武器的函数，在创建特殊武器对象时重写此函数。
    ---@deprecated 从 v1.5.3 起，此函数被 `attack` 代替。
    function Weapon:use() end

    ---开始使用该武器攻击。
    ---@deprecated 此函数功能已经整合到 `Weapon.attack` 中。
    function Weapon:start_attack()
        Mouse:press(self.attack_button)
    end

    ---停止使用该武器攻击。
    ---@deprecated 此函数功能已经整合到 `Weapon.attack` 中。
    function Weapon:stop_attack()
        Mouse:release(self.attack_button)
    end

    ---开火迭代器。
    ---@param round integer 攻击轮次。`round == 0` 时，开始开火；`round > 0` 时，持续开火（可通过返回 `nil` 提前结束）；`round < 0` 时，开火需要提前结束（必须正确处理此情形）。
    ---@param begin_timepoint integer 开始开火的时刻
    ---@return function|nil # 下一轮需要进行的开火操作，返回 `nil` 表示停止开火。
    function Weapon:fire_iterator(round, begin_timepoint)
        if round == 0 then -- 初始化扫射方向
            return function()
                Mouse:press(self.attack_button) -- 按下攻击按钮，开始攻击
            end
        end
        if round < 0 then
            return function()
                Mouse:release(self.attack_button) -- 释放攻击按钮，结束攻击
            end
        end
        if round > 0 then
            return function() end -- 保持开火状态
        end
    end

    ---扫射迭代器。
    ---@param round integer 扫射轮次。`round == 0` 时，扫射开始；`round > 0` 时，扫射可以进行（可通过返回 `nil` 提前结束）；`round < 0` 时，扫射需要提前结束（必须正确处理此情形）。
    ---@param begin_timepoint integer 扫射开始的时刻
    ---@return function|nil # 下一轮需要进行的扫射操作，返回 `nil` 表示结束当前武器的扫射。
    function Weapon:strafe_iterator(round, begin_timepoint)
        if round < 0 then
            return nil -- 强制结束扫射
        end
        
        if round == 0 then -- 初始化扫射方向
            if self.horizontal_strafe_mode == "none" then
                self.dx = 0
            elseif self.horizontal_strafe_mode == "left" then
                self.dx = math.ceil(- 50 / Setting.FIELD_IN_GAME_SENSITIVITY)
            elseif self.horizontal_strafe_mode == "right" then
                self.dx = math.ceil(50 / Setting.FIELD_IN_GAME_SENSITIVITY)
            elseif self.horizontal_strafe_mode == "random" then
                self.dx = math.ceil(Utility:random_direction() * 50 /Setting.FIELD_IN_GAME_SENSITIVITY)
            elseif self.horizontal_strafe_mode == "oscillating" then
                self.dx = function()
                    return math.ceil(math.sin(Runtime:get_running_time() / 1000) * 50 / Setting.FIELD_IN_GAME_SENSITIVITY)
                end
            end
            if self.vertical_strafe_mode == "none" then
                self.dy = 0
            elseif self.vertical_strafe_mode == "up" then
                self.dy = math.ceil(- 100 / Setting.FIELD_IN_GAME_SENSITIVITY)
            elseif self.vertical_strafe_mode == "down" then
                self.dy = math.ceil(100 / Setting.FIELD_IN_GAME_SENSITIVITY)
            elseif self.vertical_strafe_mode == "random" then
                self.dy = math.ceil(Utility:random_direction() * 100 / Setting.FIELD_IN_GAME_SENSITIVITY)
            elseif self.vertical_strafe_mode == "oscillating" then
                self.dy = function()
                    return math.ceil(math.sin(Runtime:get_running_time() / 1000) * 100 / Setting.FIELD_IN_GAME_SENSITIVITY)
                end
            end
        end
        return function()
            local dx = 0
            local dy = 0
            if type(self.dx) == "function" then
                dx = self.dx()
            else
                dx = self.dx --[[@as integer]]
            end
            if type(self.dy) == "function" then
                dy = self.dy()
            else
                dy = self.dy --[[@as integer]]
            end
            Mouse:move_relative(dx, dy, 10) -- 移动鼠标进行扫射
        end
    end

    ---使用武器进行攻击攻击。
    function Weapon:attack()
        local timepoint = Runtime:get_running_time()
        local round = 0
        repeat
            local fire = self:fire_iterator(round, timepoint)
            local strafe = self:strafe_iterator(round, timepoint)
            round = round + 1
            if fire and strafe then
                fire()
                strafe()
            else
                break
            end
        until Runtime:get_running_time() - timepoint > self.attack_duration * 1000
        local disposal
        disposal = self:fire_iterator(-1, timepoint) -- 强制结束攻击
        if disposal then disposal() end
        disposal = self:strafe_iterator(-1, timepoint) -- 强制结束扫射
        if disposal then disposal() end
    end
end -- __WEAPON_LUA__
