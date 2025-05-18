if not Player_lua then
    Player_lua = true

    Include("Keyboard.lua")
    Include("Weapon.lua")
    Include("Mouse.lua")
    Include("Runtime.lua")
    Include("Utility.lua")
    Include("DateTime.lua")
    Include("Version.lua")
    Version:set("Player", "1.5.2")

    ---@class Player
    ---@field RESPAWN_KEY string 复活或回合重置按键
    ---@field private run_direction integer 跑动方向（前后），1 表示向前，0 表示静止，-1 表示向后
    ---@field private strafe_direction integer 扫射方向（左右），1 表示向右，0 表示静止，-1 表示向左
    ---@field private last_activate_special_ability_time integer 最近一次发动角色技能的时间
    ---@field private last_weapon Weapon 最近一次使用武器
    ---@field private last_primary_weapon Weapon 最近一次使用的主武器
    ---@field private last_secondary_weapon Weapon 最近一次使用的副武器
    ---@field private last_buy_part_weapon_time integer 最近一次购买配件武器的时间
    ---@field private last_part_weapon_index integer 最近一次购买的配件武器序号
    ---@field private last_buy_special_weapon_time integer 最近一次购买特殊武器的时间
    ---@field private last_special_weapon_index integer 最近一次购买的特殊武器序号
    Player = {}

    Player.RESPAWN_KEY = Keyboard.R
    Player.run_direction = 1
    Player.last_activate_special_ability_time = 0
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

    ---在新一局游戏开始时重置玩家状态。
    function Player:reset()
        self.last_activate_special_ability_time = 0
        self.last_weapon = Weapon
        self.last_primary_weapon = Weapon
        self.last_secondary_weapon = Weapon
        self.last_buy_part_weapon_time = 0
        self.last_part_weapon_index = 0
        self.last_buy_special_weapon_time = 0
        self.last_special_weapon_index = 0
    end

    ---设置复活按键。
    ---@param key string 按键
    function Player:set_respawn_key(key)
        if not Keyboard:is_key_valid(key) then
            Error:throw({
                name = "INVALID_KEY_NAME",
                message = "无效的键盘按键",
                parameters = { key },
            })
        end
        self.RESPAWN_KEY = key
    end

    ---获取复活按键。
    ---@return string RESPAWN_KEY 复活按键
    function Player:get_respawn_key()
        return self.RESPAWN_KEY
    end

    ---@brief 创建Player实例
    ---@param init table 初始化列表
    ---@return Player player 玩家对象
    ---@remark 如果选择直接创建Player实例，则默认使用类中预定义好的若干内容
    function Player:new(init)
        local player = init or {}
        self.__index = self
        setmetatable(player, self)
        return player
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
            Keyboard:click(self.RESPAWN_KEY, 20)
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
        Keyboard:click(self.RESPAWN_KEY, 20, true) -- 回合重置
    end

    ---发动角色技能。
    function Player:activate_special_ability()
        -- 无发动角色技能的记录（一局游戏刚开始）
        if self.last_activate_special_ability_time == 0 then
            self.last_activate_special_ability_time = DateTime:get_local_timestamp()
            return
        end
        local current_time = DateTime:get_local_timestamp()
        -- 技能尚未冷却
        if
            current_time - self.last_activate_special_ability_time
            < Setting.FIELD_GAME_CHARACTER_SPECIAL_SKILLS_COOLDOWN
        then
            return
        end
        -- 按 7 发动角色技能，重复 3 次以保证成功发动
        Keyboard:click_several_times(Keyboard.SEVEN, 3, 500, Delay.SHORT)
        -- 更新最近一次发动角色技能的时间
        self.last_activate_special_ability_time = DateTime:get_local_timestamp()
    end

    function Player:buy_armor()
        if self.armor then
            self.armor:purchase()
        end
    end

    ---随机使用常规武器列表中的武器。
    function Player:attack_with_conventional_weapons()
        if not self.conventional_weapons or 0 == #self.conventional_weapons then
            return
        end
        local count = #self.conventional_weapons
        math.randomseed(Runtime:get_running_time(), DateTime:get_local_timestamp())
        -- 随机选择一件武器
        -- 若上次使用与本次随机到的武器相同，丢弃后重新购买
        local weapon = self.conventional_weapons[math.random(count)]
        -- 随机到最近一次使用的主武器相同
        if weapon.name == self.last_primary_weapon.name then
            self.last_primary_weapon:switch()
            self.last_primary_weapon:abandon()
        -- 随机到最近一次使用的副武器
        elseif weapon.name == self.last_secondary_weapon.name then
            self.last_secondary_weapon:switch()
            self.last_secondary_weapon:abandon()
        end
        -- 更新最近一次使用的主武器
        if weapon.number == Weapon.PRIMARY then
            self.last_primary_weapon = weapon
        -- 更新最近一次使用的副武器
        elseif weapon.number == Weapon.SECONDARY then
            self.last_secondary_weapon = weapon
        end
        weapon:purchase()
        weapon:switch()
        self:start_move()
        weapon:attack()
        self:stop_move()
        if Setting.SWITCH_GAME_CHARACTER_USE_SPECIAL_SKILLS then
            self:activate_special_ability()
        end
        -- 更新最近一次使用的武器
        self.last_weapon = weapon
    end

    ---按序购买配件武器。
    function Player:buy_part_weapons()
        local current_time = DateTime:get_local_timestamp()
        if
            math.abs(current_time - self.last_buy_part_weapon_time) < 20 -- 每隔 20 秒购买一次
        then
            return
        else
            self.last_buy_part_weapon_time = current_time
        end
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

    ---使用特殊武器。
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

        -- 使用武器
        self.special_weapons[index]:use()
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
        self:buy_part_weapons()
        self:attack_with_conventional_weapons()
        self:attack_with_special_weapons()
    end
end -- Player_lua
