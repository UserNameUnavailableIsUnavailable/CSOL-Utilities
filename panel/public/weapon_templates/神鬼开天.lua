Weapon:new({
    name = "神鬼开天",
    switch_delay = Delay.LONG,
    number = Weapon.MELEE,
    purchase_sequence = {},
    template_name = "神鬼开天",
    ---重写 attack 方法，按照下面定义的方式进行攻击。
    ---@param self Weapon
    attack = function (self)
        Mouse:press(Mouse.RIGHT)
        local sensitivity_x = 1 - 0.8 * math.random() -- 水平灵敏度∈(0.2, 1]
        local sensitivity_y = 1 - 0.8 * math.random() -- 竖直灵敏度∈(0.2, 1]
        local direction = Utility:random_direction() -- 随机向左或右
        local start_time = Runtime:get_running_time() -- 本次转圈开始时间
        local last_switch_time = 0
        repeat
            local current_time = Runtime:get_running_time()
            if (current_time - last_switch_time > 1000)
            then
                self:switch_without_delay()
                last_switch_time = current_time
            end
            Mouse:move_relative(math.ceil(direction * 100 * sensitivity_x / Setting.FIELD_IN_GAME_SENSITIVITY), math.ceil(math.sin(current_time / 1000) * 100 * sensitivity_y / Setting.FIELD_IN_GAME_SENSITIVITY), Delay.MINI) -- 视角运动：水平方向匀速运动，竖直方向简谐运动
        until (Runtime:get_running_time() - start_time > 6000)
        Mouse:release(Mouse.RIGHT, 200)
        Mouse:press(Mouse.LEFT, 1000)
        Keyboard:press(Weapon.reload_key, 200)
        Keyboard:release(Weapon.reload_key)
        Mouse:release(Mouse.LEFT)
    end
})