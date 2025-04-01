if (not Weapon_lua)
then
    Weapon_lua = true
    Include("Runtime.lua")
    Include("Delay.lua")
    Include("Console.lua")
    Include("Mouse.lua")
    Include("Keyboard.lua")
    Include("Version.lua")
    Version:set("Weapon", { 1, 5, 2 })
    Version:require("Weapon", "Setting", { 1, 5, 2 }, nil)

    ---@class Weapon 武器类
    ---@field NULL string 配件武器
    ---@field PRIMARY string 主武器
    ---@field SECONDARY string 副武器
    ---@field MELEE string 近战武器
    ---@field GRENADE string 手雷
    ---@field name string 武器名
    ---@field number string 武器栏位
    ---@field attack_button integer 攻击按钮
    ---@field switch_delay integer 切换延迟
    ---@field purchase_sequence table 购买按键序列
    ---@field RELOAD_KEY string 重新装填按键
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
    Weapon.RELOAD_KEY = Keyboard.R

    ---设置换弹按键。
    ---@param key string
    function Weapon:set_reload_key(key)
        if (not Keyboard:is_key_name_valid(key))
        then
            Error:throw{
                name = "INVALID_KEY_NAME",
                message = "无效的按键名称",
                parameters = { key }
            }
        end
        self.RELOAD_KEY = key
    end

    ---获取换弹按键。
    ---@return string RELOAD_KEY 换弹按键
    function Weapon:get_reload_key()
        return self.RELOAD_KEY
    end

    ---创建一个武器对象。
    ---@param init table 初始化列表
    ---@return Weapon weapon 武器对象
    function Weapon:new(init)
        local weapon = init or {}
        -- 基武器的 `__index` 指向自身
        self.__index = self
        -- 将模板武器 `weapon` 的元表设置为 `self`
        -- 这样，访问新武器中的缺省成员时，会调用模板武器的 `__index` 找到缺省值
        setmetatable(weapon, self)
        -- 参数列表有效性校验
        if (type(weapon.name) ~= "string")
        then
            Error:throw{
                name = "ILLEGAL_WEAPON_NAME_TYPE",
                message = ("武器名称必须具有 `string` 类型，但获取到的类型为 `%s`"):format(type(weapon.name)),
                parameters = {}
            }
        end
        if (type(weapon.number) ~= "string" or
            not (weapon.number == Weapon.NULL
                or weapon.number == Weapon.PRIMARY
                or weapon.number == Weapon.SECONDARY
                or weapon.number == Weapon.MELEE
                or weapon.number == Weapon.GRENADE)
        )
        then
            Error:throw{
                name = "ILLEGAL_WEAPON_NUMBER_TYPE",
                message = ("武器栏位必须为 `Weapon.NULL`、`Weapon.PRIMARY`、`Weapon.SECONDARY`、`Weapon.MELEE`、`Weapon.GRENADE`，但实际获取到的值为 `%s`"):format(weapon.number),
                parameters = {}
            }
        end
        if (not Mouse:is_button_value_valid(weapon.attack_button))
        then
            Error:throw{
                name = "ILLEGAL_WEAPON_ATTACK_BUTTON",
                message = ("武器攻击按钮必须为合法的鼠标按钮，但实际获取到的值为 `%s`"):format(weapon.attack_button),
                parameters = {}
            }
        end
        if (type(weapon.purchase_sequence) ~= "table")
        then
            Error:throw{
                name = "ILLEGAL_WEAPON_PURCHASE_SEQUENCE_TYPE",
                message = ("武器购买按键序列必须具有 `table` 类型，但实际获取到的类型为 `%s`"):format(type(weapon.purchase_sequence)),
                parameters = {}
            }
        end
        for i = 1, #weapon.purchase_sequence
        do
            if (not Keyboard:is_key_name_valid(weapon.purchase_sequence[i]))
            then
                Error:throw{
                    name = "ILLEGAL_WEAPON_PURCHASE_SEQUENCE_KEY",
                    message = ("武器购买按键序列由合法键盘按键构成，但获取到了非法按键 `%s`"):format(type(weapon.purchase_sequence[i])),
                    parameters = {}
                }
            end
        end
        if (math.floor(weapon.switch_delay) ~= weapon.switch_delay or weapon.switch_delay < 0)
        then
            Error:throw{
                name = "ILLEGAL_WEAPON_SWITCH_DELAY",
                message = "无效的武器切换延迟",
                parameters = { weapon.switch_delay }
            }
        end
        Console:information(
            "新增武器/装备：" .. weapon.name
        )
        return weapon
    end

    ---根据 `purchase_sequence` 字段中预设的按键序列购买武器。
    function Weapon:purchase()
        Runtime:push_interrupt_mask_flag()
        Runtime:disable_interrupt() -- 暂时关中断，完成原子操作
        for _, key in ipairs(self.purchase_sequence)
        do
            Keyboard:click(key, Delay.LONG, true)
        end
        Runtime:pop_interrupt_mask_flag()
        if (self.number == Weapon.GRENADE or self.number == Weapon.NULL)
        then
            Keyboard:click(Weapon.MELEE, Delay.LONG, true) -- 购买手雷后，临时切换到近战武器，防止后续鼠标点击导致使用诸如燃爆等武器。
        end
        if (not Mouse:is_cursor_position_locked())
        then
            -- 清除当前界面上的所有窗口，防止购买资金不足或关闭死亡购买界面。
            Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI, Delay.SHORT)
            Mouse:click_on(Mouse.LEFT, Setting.POSITION_GAME_ESC_MENU_CANCEL_X, Setting.POSITION_GAME_ESC_MENU_CANCEL_Y, 20) -- 点击ESC菜单的取消按钮。
        end
    end

    ---切换到指定武器。
    ---@return nil
    function Weapon:switch()
        Keyboard:click(self.number, self.switch_delay, true)
    end

    ---切换到指定武器，不考虑切枪延迟。
    ---@return nil
    function Weapon:switch_without_delay()
        Keyboard:click(self.number, 10)
    end

    ---按下 `Keyboard.G` 键丢弃武器。
    ---@return nil
    function Weapon:abandon()
        self:switch() -- 切换到指定武器。
        Keyboard:click(Keyboard.G, Delay.NORMAL)
    end

    ---确认武器购买资金不足提示框（预设按钮在 Setting.lua 中）。
    function Weapon:in_case_insufficient_funds()
        Mouse:click_on(Mouse.LEFT, Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_X, Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_Y)
        Keyboard:click(Keyboard.ZERO, Delay.SHORT)
    end

    ---关闭死亡状态下的预购买菜单（点击“重复购买”按钮，不点击“取消购买”以避免与大厅界面按钮冲突）。
    function Weapon:close_dead_purchase_menu()
        Mouse:click_on(Mouse.LEFT, Setting.POSITION_GAME_PURCHASE_BEFORE_RESPAWN_X, Setting.POSITION_GAME_PURCHASE_BEFORE_RESPAWN_Y, Delay.NORMAL)
    end

    ---使用特殊武器的函数，在创建特殊武器对象时重写此函数。
    function Weapon:use()
    end

    ---开始使用该武器攻击。
    ---@deprecated `此函数功能已经整合到 Weapon.attack` 中
    function Weapon:start_attack()
        Mouse:press(self.attack_button)
    end

    ---停止使用该武器攻击。
    ---@deprecated `此函数功能已经整合到 Weapon.attack` 中
    function Weapon:stop_attack()
        Mouse:release(self.attack_button)
    end

    ---使用武器进行攻击攻击。
    function Weapon:attack()
        Mouse:press(self.attack_button)
        local sensitivity_x = 1 - 0.8 * math.random() -- 水平灵敏度∈(0.2, 1]
        local sensitivity_y = 1 - 0.8 * math.random() -- 竖直灵敏度∈(0.2, 1]
        local direction = Utility:random_direction() -- 随机向左或右
        local start_time = Runtime:get_running_time() -- 本次转圈开始时间
        local last_switch_time = Runtime:get_running_time()
        repeat
            local current_time = Runtime:get_running_time()
            if (current_time - last_switch_time > math.random(1000, 2000))
            then
                self:switch_without_delay()
                last_switch_time = current_time
            end
            Mouse:move_relative(math.floor(direction * 100 * sensitivity_x / Setting.FIELD_IN_GAME_SENSITIVITY), math.floor(math.sin(current_time / 1000) * 100 * sensitivity_y / Setting.FIELD_IN_GAME_SENSITIVITY), 10) -- 视角运动：水平方向匀速运动，竖直方向简谐运动
        until (Runtime:get_running_time() - start_time > 7000)
        Mouse:release(self.attack_button)
    end
end -- Weapon_lua
