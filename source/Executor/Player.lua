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
Player = {}

---@brief 创建Player实例
---@param obj table 初始化列表
---@return nil
---@remark 如果选择直接创建Player实例，则默认使用类中预定义好的若干内容
function Player:new(obj)
    self.__index = self
    obj = obj or {}
    setmetatable(obj, self)
    return obj
end


---跑动方向（前后），1 表示向前，0 表示静止，-1 表示向后。
Player.run_direction = 1
---扫射方向（左右），1 表示向右，0 表示静止，-1 表示向左。
Player.strafe_direction = 1

---向随机的方向移动。
---@return nil
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

---停止移动。
---@return nil
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
---@return nil
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
---@return nil
function Player:reset_round_or_respawn()
    Keyboard:click_several_times(Keyboard.ESCAPE, 2, Delay.MINI)
    Mouse:click_on(Setting.POSITION_GAME_ESC_MENU_CANCEL_X, Setting.POSITION_GAME_ESC_MENU_CANCEL_Y, 20)
    if (Setting.KEYSTROKES_GAME_RESET_ROUND_KEY and #Setting.KEYSTROKES_GAME_RESET_ROUND_KEY > 0)
    then
        Keyboard:click_several_times(Setting.KEYSTROKES_GAME_RESET_ROUND_KEY[1], 2, Delay.SHORT) -- 回合重置
    end
end

---最近一次发动角色技能的时间。
---@type integer
Player.last_activate_special_ability_time = 0
---发动角色技能。
---@return nil
function Player:activate_special_ability()
    if (self.last_activate_special_ability_time == 0) -- 无发动角色技能的记录（一局游戏刚开始）
    then
        self.last_activate_special_ability_time = DateTime:get_local_timestamp()
        return
    end
    local current_time = DateTime:get_local_timestamp()
    if (current_time - self.last_activate_special_ability_time < Setting.FIELD_GAME_CHARACTER_SPECIAL_SKILLS_COOLDOWN) -- 技能尚未冷却
    then
        return
    end
    Keyboard:click_several_times(Keyboard.SEVEN, 3, 500) -- 按 7 发动角色技能，重复 3 次以保证成功发动
    self.last_activate_special_ability_time = DateTime:get_local_timestamp() -- 更新最近一次发动角色技能的时间。
end

---最近一次使用的主武器。
---@type Weapon
Player.last_primary_weapon = Weapon
---最近一次使用的副武器。
---@type Weapon
Player.last_secondary_weapon = Weapon
---最近一次使用的武器（主武器、副武器、近战武器）。
---@type Weapon
Player.last_weapon = Weapon

---玩家对象是否正在操作。
Player.playing_flag = false

function Player:is_playing()
    return self.playing_flag
end

---随机使用常规武器列表中的武器。
---@param weapon_list Weapon[] 常规武器列表。
---@return nil
function Player:play(weapon_list)
    self.playing_flag = true
    if (Armor) then Armor:purchase() end
    if (not weapon_list or 0 == #weapon_list)
    then
        return
    end
    local count = #weapon_list
    math.randomseed(Runtime:get_running_time(), DateTime:get_local_timestamp())
    local weapon = weapon_list[math.random(count)] -- 随机选择一件武器
    ---上次使用与本次随机到的武器相同，丢弃后重新购买
    if (weapon.name == self.last_primary_weapon.name) -- 随机到最近一次使用的主武器相同
    then
        self.last_primary_weapon:switch()
        self.last_primary_weapon:abandon()
    elseif (weapon.name == self.last_secondary_weapon.name) -- 随机到最近一次使用的副武器
    then
        self.last_secondary_weapon:switch()
        self.last_secondary_weapon:abandon()
    end
    if (weapon.number == Weapon.PRIMARY) then self.last_primary_weapon = weapon -- 更新最近一次使用的主武器
    elseif (weapon.number == Weapon.SECONDARY) then self.last_secondary_weapon = weapon end -- 更新最近一次使用的副武器
    weapon:purchase()
    weapon:switch()
    self:start_move()
    weapon:attack()
    self:stop_move()
    self:activate_special_ability()
    self.last_weapon = weapon --更新最近一次使用的武器
    self.playing_flag = false
end

---上次购买配件武器的时间。
---@type integer
Player.last_buy_part_weapon_time = 0
---循环计数器，用于记录配件武器购买序号。
---@type function | nil
Player.part_weapon_counter = nil
---上次购买的配件武器。
---@type integer
Player.last_part_weapon_index = 0
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
    if (not weapons or #weapons == 0) -- 非空且至少有一件武器
    then
        return
    end
    local index = Player.last_part_weapon_index + 1
    if (index > #weapons)
    then
        index = 1
    end
    local weapon = weapons[index]
    weapon:purchase() -- 购买配件武器
    Player.last_part_weapon_index = index
end

---上次购买特殊武器的时间。
---@type integer
Player.last_buy_special_weapon_time = 0
Player.last_special_weapon_index = 0
---@param special_weapons Weapon[] 特殊武器。
function Player:use_special_weapon(special_weapons)
    Player.playing_flag = true
    if (not special_weapons or #special_weapons == 0) -- 非空且至少有一件武器
    then
        return
    end

    local current_time = DateTime:get_local_timestamp()
    local index = Player.last_special_weapon_index + 1 -- 下一件武器

    if (index > #special_weapons)
    then
        index = 1
    end

    if (current_time - self.last_buy_special_weapon_time > 20) -- 每隔 20 秒购买一次特殊武器
    then
        special_weapons[index]:purchase()
        self.last_buy_special_weapon_time = current_time
    end

    special_weapons[index]:use() -- 使用武器
    Player.last_part_weapon_index = index
    if (self.last_weapon)
    then
        self.last_weapon:switch() -- 使用后切换回原来的武器
    end
    Player.playing_flag = false
end

---在新一局游戏开始时重置玩家状态。
function Player:reset()
    self.last_primary_weapon = Weapon
    self.last_secondary_weapon = Weapon
    self.last_buy_part_weapon_time = 0
    self.part_weapon_counter = nil
    self.last_buy_special_weapon_time = 0
    self.last_activate_special_ability_time = 0
end

end -- Play_lua
