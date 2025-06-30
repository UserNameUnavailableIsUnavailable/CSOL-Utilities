Weapon:new({
    name = "【幽浮】控制核心",
    purchase_sequence = {},
    switch_delay = 750,
    number = Weapon.GRENADE,
    template_name = "【幽浮】控制核心",
    COOL_DOWN_TIME = 60,
    last_start_moment = 0,
    ---为该武器重写 \`use\` 方法。
    ---@param self Weapon
    attack = function (self)
        local current_time = DateTime:get_local_timestamp()
        if (current_time - self.last_start_moment > self.COOL_DOWN_TIME)
        then
            self:switch()
            self.last_start_moment = current_time
            Mouse:click(Mouse.LEFT, 500)
        end
    end
})
