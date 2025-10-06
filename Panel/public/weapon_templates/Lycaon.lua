---夙狼夜契（Lycaon）武器模板
---@class Lycaon : Weapon
---@field last_phantom_rush_timepoint integer 上一次突袭的时刻
---@field garrison boolean 是否在驻守状态
---@field cooldown_time integer 冷却时间
Weapon:new({
    name = "夙狼夜契",
    switch_delay = 600,
    purchase_sequence = {},
    number = Weapon.GRENADE,
    template_name = "Lycaon",
    last_phantom_rush_timepoint = 0,
    garrison = false,
    cooldown_time = 30,
    ---@param self Lycaon
    attack = function(self)
        local current_tp = DateTime:get_local_timestamp()
            if current_tp - self.last_phantom_rush_timepoint > self.cooldown_time then
                self:switch()
                Mouse:click(Mouse.RIGHT, 500)
                Keyboard:click(self:get_reload_key(), 500)
                self.last_phantom_rush_timepoint = current_tp
            end
    end
})