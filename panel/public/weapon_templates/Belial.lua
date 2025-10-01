---炽翼魔印（Belial）模板
---@class Belial : Weapon
---@field lightening boolean 是否在释放光印
---@field lightening_start_timepoint integer 光印释放的时刻
---@field charge_start_timepoint integer 充能开始的时刻
---@field LIGHTENING_TIME integer 光印释放时间
---@field CHARGE_TIME integer 充能时间
Weapon:new({
    name = "炽翼魔印",
    switch_delay = 850,
    number = Weapon.GRENADE ,
    purchase_sequence = {},
    template_name = "Belial",
    lightening = false, -- 是否在释放光印
    lightening_start_timepoint = 0, --  光印释放的时刻。
    charge_start_timepoint = 0, -- 充能开始的时刻。
    LIGHTENING_TIME = 25, -- 光印释放时间。
    CHARGE_TIME = 10, -- 充能时间。
    ---为该武器重写 `use` 方法。
    ---@param self Belial
    attack = function (self)
        local current_time = DateTime:get_local_timestamp() -- 当前时间戳
        -- 当前正在充能，且充能时间超过 `CHARGE_TIME`
        if (not self.lightening and current_time - self.charge_start_timepoint > self.CHARGE_TIME)
        then
            self.lightening = true
            self.lightening_start_timepoint = current_time
            self:switch()
        -- 当前正在释放光印，且释放时间超过 `LIGHTENING_TIME`
        elseif (self.lightening and current_time - self.lightening_start_timepoint > self.LIGHTENING_TIME)
        then
            self.lightening = false
            self.charge_start_timepoint = current_time
            self:switch()
            Mouse:move_relative(0, 4000 / Setting.FIELD_IN_GAME_SENSITIVITY, Delay.NORMAL)
            Keyboard:click(Weapon.reload_key, Delay.LONG_LONG)
            Mouse:move_relative(0, -4000 / Setting.FIELD_IN_GAME_SENSITIVITY, Delay.NORMAL)
        end
    end
})
