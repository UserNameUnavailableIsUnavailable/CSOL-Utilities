Weapon:new({
    name = "炽翼魔印",
    switch_delay = 850,
    number = Weapon.GRENADE ,
    purchase_sequence = {},
    template_name = "炽翼魔印",
    discharging = false, -- 是否在释放光印
    discharge_start_moment = 0, --  光印释放的时刻。
    charge_start_moment = 0, -- 充能开始的时刻。
    DISCHARGE_TIME = 25, -- 光印释放时间。
    RECHARGE_TIME = 10, -- 充能时间。
    ---为该武器重写 `use` 方法。
    ---@param self Weapon
    attack = function (self)
        local current_time = DateTime:get_local_timestamp() -- 当前时间戳
        -- 当前正在充能，且充能时间超过 `RECHARGE_TIME`。
        if (not self.discharging and current_time - self.charge_start_moment > self.RECHARGE_TIME)
        then
            self.discharging = true
            self.discharge_start_moment = current_time
            self:switch()
        -- 当前正在释放，且释放时间超过 `DISCHARGE_TIME`。
        elseif (self.discharging and current_time - self.charge_start_moment > self.DISCHARGE_TIME)
        then
            self.discharging = false
            self.charge_start_moment = current_time
            self:switch()
            Mouse:move_relative(0, 4000 / Setting.FIELD_IN_GAME_SENSITIVITY, Delay.NORMAL)
            Keyboard:click(Weapon.RELOAD_KEY, Delay.LONG_LONG)
            Mouse:move_relative(0, -4000 / Setting.FIELD_IN_GAME_SENSITIVITY, Delay.NORMAL)
        end
    end
})
