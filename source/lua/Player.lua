if (not Player_lua)
then
Player_lua = true
Load("Keyboard.lua")
Load("Weapon.lua")
Load("Mouse.lua")
Load("Runtime.lua")
Load("Utility.lua")
Load("DateTime.lua")
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
---@return nil
function Player:turn()
    local sensitivity = (2 - math.random()) / 2 -- 灵敏度∈(0.5, 1]
    local direction = Utility:random_direction() -- 随机向左或右
    local start_time = DateTime:get_local_timestamp()
    repeat
        Mouse:move_relative(100 * direction * sensitivity, 0, Delay.MINI)
    until (DateTime:get_local_timestamp() - start_time > 6)
    Keyboard:click(Keyboard.R, Delay.LONG) -- 回合重置
end

---上一次使用的武器。
---@type Weapon
Player.last_weapon = nil
---使用武器。
---@param weapon_list Weapon[] 武器列表。
---@return nil
function Player:play(weapon_list)
    if (not weapon_list or 0 == #weapon_list)
    then
        return
    end
    local count = #weapon_list
    local weapon = weapon_list[math.random(count)] -- 随机选择一件武器
    ---上次使用的武器非近战或手雷，则先将其丢弃再购买选中的武器
    if (self.last_weapon and self.last_weapon.number ~= Weapon.MELEE and self.last_weapon.number ~= Weapon.GRENADE)
    then
        self.last_weapon:abandon()
    end
    AC:purchase()
    weapon:purchase()
    weapon:switch()
    self:start_move()
    weapon:start_attack()
    self:turn()
    weapon:stop_attack()
    self:stop_move()
    self.last_weapon = weapon -- 更新上一次使用的武器
end

---上次购买配件武器的时间。
---@type integer
Player.last_buy_part_weapon_time = 0
---循环计数器，用于记录配件武器购买序号。
---@type function | nil
Player.part_weapon_counter = nil
---按序购买配件武器。
---@param weapon_list Weapon[] 配件武器列表。
function Player:buy_part_weapon(weapon_list)
    local current_time = DateTime:get_local_timestamp()
    if (math.abs(current_time - self.last_buy_part_weapon_time) < 30) -- 每隔 20 秒购买一次
    then
        return
    else
        self.last_buy_part_weapon_time = current_time
    end
    if (not weapon_list or #weapon_list == 0)
    then
        return
    end
    if (not self.part_weapon_counter)
    then
        self.part_weapon_counter = Utility:create_counter(1, #weapon_list)
    end
    local weapon_index = self.part_weapon_counter()
    local weapon = weapon_list[weapon_index]
    weapon:purchase() -- 购买配件武器
end

---上次购买特殊武器的时间。
---@type integer
Player.last_buy_special_weapon_time = 0
---@param weapon Weapon 特殊武器。
function Player:use_special_weapon(weapon)
    if (not weapon) then return end
    local current_time = DateTime:get_local_timestamp()
    if (math.abs(current_time - self.last_buy_special_weapon_time) < 20) -- 每隔 30 秒购买一次
    then
        return
    else
        weapon:purchase()
        self.last_buy_special_weapon_time = current_time
    end
    weapon:use()
end

---通过多次按下 `Keyboard.ESCAPE` 清除当前界面上的所有窗口。
---@return nil
function Player:clear_UI()
    Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.NORMAL)
    Mouse:double_click_on(Setting.ZS_GAME_ESC_MENU_CANCEL_X, Setting.ZS_GAME_ESC_MENU_CANCEL_Y) -- 点击ESC菜单的取消按钮
    Mouse:double_click_on(Setting.GAME_ROUND_CONFIRM_X, Setting.GAME_ROUND_CONFIRM_Y) -- 结算界面确认
end
end -- Play_lua