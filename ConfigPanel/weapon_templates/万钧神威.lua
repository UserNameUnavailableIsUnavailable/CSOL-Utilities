Weapon:new({
    name = "万钧神威",
    switch_delay = Delay.LONG,
    number = Weapon.MELEE,
    purchase_sequence = {},
    template_name = "万钧神威",
    ---重写 attack 方法，按照下面定义的方式进行攻击。
    ---若您需要使用万钧神威进行挂机且没有编程经验，则请勿修改此函数。
    ---@param self Weapon
    attack = function (self)
        Mouse:press(Mouse.RIGHT) -- 按下鼠标右键进行范围攻击
        local sensitivity_x = 1 - 0.8 * math.random() -- 水平灵敏度∈(0.2, 1]
        local sensitivity_y = 1 - 0.8 * math.random() -- 竖直灵敏度∈(0.2, 1]
        local direction = Utility:random_direction() -- 随机向左或右
        local start_time = Runtime:get_running_time() -- 本次转圈开始时间
        local last_throw_time = Runtime:get_running_time()
        repeat
            local current_time = Runtime:get_running_time()
            Mouse:move_relative( -- 视角运动：水平方向匀速运动，竖直方向简谐运动
                math.ceil(direction * 100 * sensitivity_x / Setting.FIELD_IN_GAME_SENSITIVITY),
                math.ceil(math.sin(current_time / 1000) * 100 * sensitivity_y / Setting.FIELD_IN_GAME_SENSITIVITY),
                Delay.MINI)
            if (Runtime:get_running_time() - last_throw_time > 4000)
            then
                Keyboard:click(Weapon.RELOAD_KEY, Delay.MINI)
                last_throw_time = Runtime:get_running_time()
            end
        until (Runtime:get_running_time() - start_time > 9000)
        Mouse:release(Mouse.RIGHT) -- 松开鼠标右键释放旋风
    end
})