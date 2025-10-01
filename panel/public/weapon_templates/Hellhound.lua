---【幽浮】控制核心（Hellhound）模板
---@class Hellhound : Weapon
---@field COOL_DOWN_TIME integer 冷却时间
---@field last_call_drone_timepoint integer 上一次召唤无人机的时刻
Weapon:new({
    name = "【幽浮】控制核心",
    purchase_sequence = {},
    switch_delay = 750,
    number = Weapon.GRENADE,
    template_name = "Hellhound",
    COOL_DOWN_TIME = 60,
    last_call_drone_timepoint = 0,
    ---@param self Hellhound
    attack = function (self)
        local current_time = DateTime:get_local_timestamp()
        if (current_time - self.last_call_drone_timepoint > self.COOL_DOWN_TIME)
        then
            self:switch()
            self.last_call_drone_timepoint = current_time
            Mouse:click(Mouse.LEFT, 500)
        end
    end
})
