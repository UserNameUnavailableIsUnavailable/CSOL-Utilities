if (not Player_lua)
then
    Player_lua = true
    Include("Keyboard.lua")
    Include("Weapon.lua")
    Include("Mouse.lua")
    Include("Runtime.lua")
    Include("Utility.lua")
    Include("DateTime.lua")
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
    Player.strafe_direction = 1
    Player.last_activate_special_ability_time = 0
    Player.last_weapon = Weapon
    Player.last_primary_weapon = Weapon
    Player.last_secondary_weapon = Weapon
    Player.last_buy_part_weapon_time = 0
    Player.last_part_weapon_index = 0
    Player.last_buy_special_weapon_time = 0
    Player.last_special_weapon_index = 0

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
        if (not Keyboard:is_key_name_valid(key))
        then
           self("Invalid key name.")
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
    ---@return nil
    ---@remark 如果选择直接创建Player实例，则默认使用类中预定义好的若干内容
    function Player:new(init)
        self.__index = self
        local player = init or {}
        setmetatable(player, self)
        return player
    end


    ---向随机的方向移动
    function Player:start_move()
        self.run_direction = math.random(3) - 2
        self.strafe_direction = math.random(3) - 2

        if (self.run_direction == 1)
        then
            Keyboard:press(Keyboard.W, Delay.SHORT)
        elseif (self.run_direction == -1)
        then
            Keyboard:press(Keyboard.S, Delay.SHORT)
        end

        if (self.strafe_direction == 1)
        then
            Keyboard:press(Keyboard.D, Delay.SHORT)
        elseif (self.strafe_direction == -1)
        then
            Keyboard:press(Keyboard.A, Delay.SHORT)
        end
    end

    ---停止移动
    function Player:stop_move()
        if (self.run_direction == 1)
        then
            Keyboard:release(Keyboard.W, Delay.SHORT)
        elseif (self.run_direction == -1)
        then
            Keyboard:release(Keyboard.S, Delay.SHORT)
        end

        if (self.strafe_direction == 1)
        then
            Keyboard:release(Keyboard.D, Delay.SHORT)
        elseif (self.strafe_direction == -1)
        then
            Keyboard:release(Keyboard.A, Delay.SHORT)
        end
    end

    ---移动视角，并执行回合重置。
    ---@deprecated 此函数功能已经合并到 `Weapon.attack` 函数中
    function Player:turn()
        local sensitivity_x = 1 - 0.8 * math.random() -- 水平灵敏度 ∈ (0.2, 1]
        local sensitivity_y = 1 - 0.8 * math.random() -- 竖直灵敏度 ∈ (0.2, 1]
        local direction = Utility:random_direction() -- 随机向左或右
        local start_time = DateTime:get_local_timestamp() -- 本次转圈开始时间
        repeat
            local t = Runtime:get_running_time() / 1000
            Mouse:move_relative(math.floor(direction * 100 * sensitivity_x), math.floor(math.sin(t) * 100 * sensitivity_y), Delay.MINI) -- 视角运动：水平方向匀速，竖直方向简谐
        until (DateTime:get_local_timestamp() - start_time > 6)
    end

    ---回合重置或复活。
    function Player:reset_round_or_respawn()
        Runtime:sleep(20, true) -- 等待鼠标光标位置稳定（精确定时，尽可能减少对正常挂机的影响）
        local x, y = Mouse:locate() -- 获取光标位置
        Mouse:move_relative(250, 20, 20, true) -- 小幅度移动光标
        local _x, _y = Mouse:locate() -- 稳定后再次获取光标位置
        -- 如果光标位置在中心附近，且变化量比移动幅度小，则说明当前没有其他弹窗的影响，直接按下回合重置/复活按键
        if (math.abs(_x - x) < 200 and math.abs(_y - y) < 200 and math.abs(_x - 32767) < 250 and math.abs(_x - 32767) < 250)
        then
            Keyboard:click(Keyboard.R, 10, true)
            return
        end
        -- 存在其他弹窗的影响
        Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI) -- 清除弹窗
        Mouse:click_on(Mouse.LEFT, Setting.POSITION_GAME_ESC_MENU_CANCEL_X, Setting.POSITION_GAME_ESC_MENU_CANCEL_Y, Delay.SHORT) -- 点击弹窗中的 “取消” 按钮
        Keyboard:click(self.RESPAWN_KEY, Delay.MINI) -- 回合重置
    end

    ---发动角色技能。
    function Player:activate_special_ability()
        -- 无发动角色技能的记录（一局游戏刚开始） 
        if (self.last_activate_special_ability_time == 0)
        then
            self.last_activate_special_ability_time = DateTime:get_local_timestamp()
            return
        end
        local current_time = DateTime:get_local_timestamp()
        -- 技能尚未冷却
        if (current_time - self.last_activate_special_ability_time < Setting.FIELD_GAME_CHARACTER_SPECIAL_SKILLS_COOLDOWN)
        then
            return
        end
        -- 按 7 发动角色技能，重复 3 次以保证成功发动
        Keyboard:click_several_times(Keyboard.SEVEN, 3, 500)
        -- 更新最近一次发动角色技能的时间
        self.last_activate_special_ability_time = DateTime:get_local_timestamp()
    end

    ---随机使用常规武器列表中的武器。
    ---@param weapon_list Weapon[] 常规武器列表。
    function Player:play(weapon_list)
        if (Armor) then Armor:purchase() end
        if (not weapon_list or 0 == #weapon_list)
        then
            return
        end
        local count = #weapon_list
        math.randomseed(Runtime:get_running_time(), DateTime:get_local_timestamp())
        -- 随机选择一件武器
        -- 若上次使用与本次随机到的武器相同，丢弃后重新购买
        local weapon = weapon_list[math.random(count)]
        -- 随机到最近一次使用的主武器相同
        if (weapon.name == self.last_primary_weapon.name)
        then
            self.last_primary_weapon:switch()
            self.last_primary_weapon:abandon()
        -- 随机到最近一次使用的副武器
        elseif (weapon.name == self.last_secondary_weapon.name)
        then
            self.last_secondary_weapon:switch()
            self.last_secondary_weapon:abandon()
        end
        -- 更新最近一次使用的主武器
        if (weapon.number == Weapon.PRIMARY) then self.last_primary_weapon = weapon
        -- 更新最近一次使用的副武器
        elseif (weapon.number == Weapon.SECONDARY) then self.last_secondary_weapon = weapon end
        weapon:purchase()
        weapon:switch()
        self:start_move()
        weapon:attack()
        self:stop_move()
        self:activate_special_ability()
        -- 更新最近一次使用的武器
        self.last_weapon = weapon
    end

    ---按序购买配件武器。
    ---@param weapons Weapon[] 配件武器列表。
    function Player:buy_part_weapon(weapons)
        local current_time = DateTime:get_local_timestamp()
        if (math.abs(current_time - self.last_buy_part_weapon_time) < 20) -- 每隔 20 秒购买一次
        then
            return
        else
            self.last_buy_part_weapon_time = current_time
        end
        -- 非空且至少有一件武器
        if (not weapons or #weapons == 0)
        then
            return
        end
        local index = Player.last_part_weapon_index + 1
        if (index > #weapons)
        then
            index = 1
        end
        local weapon = weapons[index]
        -- 购买配件武器
        weapon:purchase()
        Player.last_part_weapon_index = index
    end

    ---@param special_weapons Weapon[] 特殊武器。
    function Player:use_special_weapon(special_weapons)
        Player.playing_flag = true
        -- 非空且至少有一件武器
        if (not special_weapons or #special_weapons == 0)
        then
            return
        end

        local current_time = DateTime:get_local_timestamp()
        -- 下一件武器
        local index = Player.last_special_weapon_index + 1

        if (index > #special_weapons)
        then
            index = 1
        end

        -- 每隔 20 秒购买一次特殊武器
        if (current_time - self.last_buy_special_weapon_time > 20)
        then
            special_weapons[index]:purchase()
            self.last_buy_special_weapon_time = current_time
        end

        -- 使用武器
        special_weapons[index]:use()
        Player.last_part_weapon_index = index
        if (self.last_weapon)
        then
            -- 使用后切换回原来的武器
            self.last_weapon:switch()
        end
        Player.playing_flag = false
    end

end -- Play_lua
