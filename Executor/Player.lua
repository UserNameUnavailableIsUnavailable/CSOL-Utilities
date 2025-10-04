if not Player_lua then
    Player_lua = true

    Include("Keyboard.lua")
    Include("Weapon.lua")
    Include("Mouse.lua")
    Include("Interrupt.lua")
    Include("Runtime.lua")
    Include("Utility.lua")
    Include("DateTime.lua")
    Include("Version.lua")
    Version:set("Player", "1.5.4")

    ---@class Player
    ---@field respawn_key KEYBOARD_KEY 复活或回合重置按键
    ---@field private run_direction integer 跑动方向（前后），1 表示向前，0 表示静止，-1 表示向后
    ---@field private strafe_direction integer 扫射方向（左右），1 表示向右，0 表示静止，-1 表示向左
    ---@field private last_activate_special_skill_time integer 最近一次发动角色技能的时间
    ---@field private last_weapon Weapon 最近一次使用武器
    ---@field private last_primary_weapon Weapon 最近一次使用的主武器
    ---@field private last_secondary_weapon Weapon 最近一次使用的副武器
    ---@field private last_buy_part_weapon_time integer 最近一次购买配件武器的时间
    ---@field private last_part_weapon_index integer 最近一次购买的配件武器序号
    ---@field private last_buy_special_weapon_time integer 最近一次购买特殊武器的时间
    ---@field private last_special_weapon_index integer 最近一次购买的特殊武器序号
    ---@field private current_weapon Weapon|nil 当前使用的武器
    Player = {}

    ---@class PlayerInitializer
    ---@field respawn_key? KEYBOARD_KEY
    ---@field armor? Weapon
    ---@field part_weapons? Weapon[]
    ---@field conventional_weapons? Weapon[]
    ---@field special_weapons? Weapon[]
    ---@field current_weapon? Weapon|nil

    Player.respawn_key = Keyboard.R
    Player.run_direction = 1
    Player.last_activate_special_skill_time = 0
    Player.last_weapon = Weapon
    Player.last_primary_weapon = Weapon
    Player.last_secondary_weapon = Weapon
    Player.last_buy_part_weapon_time = 0
    Player.last_part_weapon_index = 0
    Player.last_buy_special_weapon_time = 0
    Player.last_special_weapon_index = 0
    Player.armor = nil
    Player.part_weapons = {}
    Player.conventional_weapons = {}
    Player.special_weapons = {}
    Player.current_weapon = nil

    ---在新一局游戏开始时重置玩家状态。
    function Player:reset()
        self.last_activate_special_skill_time = 0
        self.last_weapon = Weapon
        self.last_primary_weapon = Weapon
        self.last_secondary_weapon = Weapon
        self.last_buy_part_weapon_time = 0
        self.last_part_weapon_index = 0
        self.last_buy_special_weapon_time = 0
        self.last_special_weapon_index = 0
        self.current_weapon = nil
    end

    ---获取当前使用的武器。
    ---@return Weapon|nil current_weapon 当前使用的武器
    function Player:get_current_weapon()
        return self.current_weapon
    end

    ---设置当前使用的武器。
    ---@param weapon Weapon|nil 当前使用的武器
    function Player:set_current_weapon(weapon)
        self.current_weapon = weapon
    end

    ---设置复活按键。
    ---@param key KEYBOARD_KEY 按键
    function Player:set_respawn_key(key)
        self.respawn_key = key
    end

    ---获取复活按键。
    ---@return KEYBOARD_KEY respawn_key 复活按键
    function Player:get_respawn_key()
        return self.respawn_key
    end

    ---@brief 创建 Player 实例
    ---@param init PlayerInitializer 初始化列表
    ---@return Player player 玩家对象
    ---@remark 如果选择直接创建 Player 实例，则默认使用类中预定义好的若干内容
    function Player:new(init)
        local player = init
        self.__index = self
        setmetatable(player, self)
        return player --[[@as Player]]
    end

    ---向随机的方向移动
    function Player:start_move()
        math.randomseed(Runtime:get_running_time())
        self.run_direction = math.random(0, 4)
        if self.run_direction == 1 then
            Keyboard:press(Keyboard.W, Delay.MINI)
        elseif self.run_direction == 2 then
            Keyboard:press(Keyboard.A, Delay.MINI)
        elseif self.run_direction == 3 then
            Keyboard:press(Keyboard.S, Delay.MINI)
        elseif self.run_direction == 4 then
            Keyboard:press(Keyboard.D, Delay.MINI)
        end
    end

    ---停止移动
    function Player:stop_move()
        if self.run_direction == 1 then
            Keyboard:release(Keyboard.W, Delay.SHORT)
        elseif self.run_direction == 2 then
            Keyboard:release(Keyboard.A, Delay.SHORT)
        elseif self.run_direction == 3 then
            Keyboard:release(Keyboard.S, Delay.SHORT)
        elseif self.run_direction == 4 then
            Keyboard:release(Keyboard.D, Delay.SHORT)
        end
    end

    ---移动视角，并执行回合重置。
    ---@deprecated 此函数功能已经合并到 `Weapon.attack` 函数中
    function Player:view()
        local sensitivity_x = 1 - 0.8 * math.random() -- 水平灵敏度 ∈ (0.2, 1]
        local sensitivity_y = 1 - 0.8 * math.random() -- 竖直灵敏度 ∈ (0.2, 1]
        local direction = Utility:random_direction() -- 随机向左或右
        local start_time = DateTime:get_local_timestamp() -- 本次转圈开始时间
        repeat
            local t = Runtime:get_running_time() / 1000
            Mouse:move_relative(
                math.ceil(direction * 100 * sensitivity_x),
                math.ceil(math.sin(t) * 100 * sensitivity_y),
                Delay.MINI
            ) -- 视角运动：水平方向匀速，竖直方向简谐
        until DateTime:get_local_timestamp() - start_time > 6
    end

    ---回合重置或复活。
    function Player:reset_round_or_respawn()
        if Mouse:is_cursor_position_locked() then
            Keyboard:click(self.respawn_key, 50)
            return
        end
        -- 存在其他弹窗的影响
        Keyboard:click_several_times(Keyboard.ESCAPE, 4, 10, 25, true) -- 清除弹窗
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_GAME_ESC_MENU_CANCEL_X,
            Setting.POSITION_GAME_ESC_MENU_CANCEL_Y,
            30,
            true
        )
        Keyboard:click(self.respawn_key, 50, true) -- 回合重置
    end

    ---发动角色技能。
    function Player:activate_special_skill()
        -- 无发动角色技能的记录（一局游戏刚开始）
        if self.last_activate_special_skill_time == 0 then
            self.last_activate_special_skill_time = DateTime:get_local_timestamp()
            return
        end
        local current_time = DateTime:get_local_timestamp()
        -- 技能尚未冷却
        if
            current_time - self.last_activate_special_skill_time
            < Setting.FIELD_GAME_CHARACTER_SPECIAL_SKILLS_COOLDOWN
        then
            return
        end
        -- 按 7 发动角色技能，重复 3 次以保证成功发动
        Keyboard:click_several_times(Keyboard.SEVEN, 3, 150, Delay.SHORT)
        -- 更新最近一次发动角色技能的时间
        self.last_activate_special_skill_time = DateTime:get_local_timestamp()
    end

    function Player:buy_armor()
        if self.armor then
            self.armor:purchase()
        end
    end

    ---重新购买需要换弹的武器。
    ---@param weapon Weapon
    local function repurchase_reloading_required_weapon(weapon)
        if weapon.reloading_required then
            weapon:switch() -- 切换到该武器
            weapon:abandon() -- 丢弃
        end
        weapon:purchase() -- 重新购买
        weapon:switch() -- 切换到购买的武器
    end

    ---随机购买并使用常规武器列表中的武器进行攻击。
    function Player:attack_with_conventional_weapons()
        if not self.conventional_weapons or 0 == #self.conventional_weapons then
            return
        end
        local count = #self.conventional_weapons
        math.randomseed(Runtime:get_running_time(), DateTime:get_local_timestamp())
        -- 随机选择一件武器
        local weapon = self.conventional_weapons[math.random(count)]
        if
            -- 随机到最近一次使用的主武器
            weapon.number == Weapon.PRIMARY and weapon.name == self.last_primary_weapon.name or
             -- 随机到最近一次使用的副武器
            weapon.number == Weapon.SECONDARY and weapon.name == self.last_secondary_weapon.name
        then
            repurchase_reloading_required_weapon(weapon)
        else -- 其余武器正常购买
            weapon:purchase() -- 购买武器
            weapon:switch() -- 切换到购买的武器
        end
        -- 更新最近一次使用的主武器
        if weapon.number == Weapon.PRIMARY then
            self.last_primary_weapon = weapon
        -- 更新最近一次使用的副武器
        elseif weapon.number == Weapon.SECONDARY then
            self.last_secondary_weapon = weapon
        end
        self:start_move() -- 开始移动
        self:set_current_weapon(weapon) -- 记录当前使用的武器
        weapon:attack() -- 使用武器攻击
        self:set_current_weapon(nil) -- 攻击结束后，清空当前使用的武器记录
        self:stop_move() -- 结束移动
        if Setting.SWITCH_GAME_CHARACTER_USE_SPECIAL_SKILLS then
            self:activate_special_skill() -- 激活角色特殊技能
        end
        -- 更新最近一次使用的武器
        self.last_weapon = weapon
    end

    ---按序购买配件武器。
    function Player:buy_part_weapons()
        -- 非空且至少有一件武器
        if not self.part_weapons or #self.part_weapons == 0 then
            return
        end
        local index = Player.last_part_weapon_index + 1
        if index > #self.part_weapons then
            index = 1
        end
        local weapon = self.part_weapons[index]
        -- 购买配件武器
        weapon:purchase()
        Player.last_part_weapon_index = index
    end

    ---按序购买并使用特殊武器列表中的武器进行攻击。
    function Player:attack_with_special_weapons()
        Player.playing_flag = true
        -- 非空且至少有一件武器
        if not self.special_weapons or #self.special_weapons == 0 then
            return
        end

        local current_time = DateTime:get_local_timestamp()
        -- 下一件武器
        local index = Player.last_special_weapon_index + 1

        if index > #self.special_weapons then
            index = 1
        end

        -- 每隔 20 秒购买一次特殊武器
        if current_time - self.last_buy_special_weapon_time > 20 then
            self.special_weapons[index]:purchase()
            self.last_buy_special_weapon_time = current_time
        end
        self:set_current_weapon(self.special_weapons[index]) -- 记录当前使用的武器
        -- 使用武器
        self.special_weapons[index]:attack()
        self:set_current_weapon(nil) -- 攻击结束后，清空当前使用的武器记录
        Player.last_part_weapon_index = index
        if self.last_weapon then
            -- 使用后切换回原来的武器
            self.last_weapon:switch()
        end
        Player.playing_flag = false
    end

    ---使用玩家对象进行游戏。
    function Player:play()
        self:buy_armor()
        local current_time = DateTime:get_local_timestamp()
        if
            math.abs(current_time - self.last_buy_part_weapon_time) > 30 -- 每隔 30 秒购买一次
        then
            self:buy_part_weapons()
            self.last_buy_part_weapon_time = current_time
        end
        self:attack_with_conventional_weapons()
        self:attack_with_special_weapons()
    end
end -- Player_lua
