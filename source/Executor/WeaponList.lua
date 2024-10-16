---有关本文件的配置说明在手册第 3 章---
if (not WeaponList_lua)
then
WeaponList_lua = true

-- 配件武器连同默认武器一同使用。
-- 扩展武器连同特殊武器一同使用。

---配件武器列表。
---@type Weapon[]
PartWeaponList = {
    Weapon:new({
        name = "星战前线·加特林",
        number = Weapon.PRIMARY,
        purchase_sequence = { Keyboard.B, Keyboard.J, Keyboard.TWO, Keyboard.FOUR }
    }),
    Weapon:new({
        name = "FNP-45战损版",
        number = Weapon.SECONDARY,
        purchase_sequence = { Keyboard.B, Keyboard.J, Keyboard.ONE, Keyboard.TWO }
    }),
    Weapon:new({
        name = "燃爆Ignite-10",
        number = Weapon.GRENADE,
        purchase_sequence = { Keyboard.B, Keyboard.J, Keyboard.EIGHT, Keyboard.SEVEN }
    })
}

---默认武器列表。
---@type Weapon[]
DefaultWeaponList = {
    Weapon:new{
        name = "幻境！光棱剑",
        switch_delay = Delay.NORMAL,
        number = Weapon.MELEE,
        purchase_sequence = { Keyboard.B, Keyboard.G },
        attack_button = Mouse.LEFT
    }
}

---扩展武器列表。
---@type Weapon[]
ExtendedWeaponList = {
    Weapon:new{
        name = "神鬼开天/魔神开天",
        switch_delay = Delay.SHORT,
        number = Weapon.MELEE,
        purchase_sequence = { Keyboard.B, Keyboard.NINE, Keyboard.SIX },
        ---重写 attack 方法，按照下面定义的方式进行攻击。
        ---若您使用神鬼开天/魔神开天进行挂机且没有编程经验，则请勿修改此函数。
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
                Mouse:move_relative(math.floor(direction * 100 * sensitivity_x), math.floor(math.sin(current_time / 1000) * 100 * sensitivity_y), Delay.MINI) -- 视角运动：水平方向匀速运动，竖直方向简谐运动
            until (Runtime:get_running_time() - start_time > 6000)
            Mouse:release(Mouse.RIGHT, 200)
            Mouse:press(Mouse.LEFT, 1000)
            Keyboard:press(Keyboard.R, 200)
            Keyboard:release(Keyboard.R)
            Mouse:release(Mouse.LEFT)
        end
    },
	Weapon:new{
        name = "万钧神威",
        switch_delay = Delay.SHORT,
        number = Weapon.MELEE,
        purchase_sequence = { Keyboard.B, Keyboard.NINE, Keyboard.FOUR },
        ---重写 attack 方法，按照下面定义的方式进行攻击。
        ---若您需要使用万钧神威进行挂机且没有编程经验，则请勿修改此函数。
        ---@param self Weapon
        attack = function (self)
            Mouse:press(Mouse.RIGHT) -- 按下鼠标右键进行范围攻击
            local sensitivity_x = 1 - 0.8 * math.random() -- 水平灵敏度∈(0.2, 1]
            local sensitivity_y = 1 - 0.8 * math.random() -- 竖直灵敏度∈(0.2, 1]
            local direction = Utility:random_direction() -- 随机向左或右
            local start_time = Runtime:get_running_time() -- 本次转圈开始时间
            local last_switch_time = 0
            local first_throw = false
            local second_throw = false
            repeat
                local current_time = Runtime:get_running_time()
                if (current_time - last_switch_time > 1000)
                then
                    self:switch_without_delay()
                    last_switch_time = current_time
                end
                Mouse:move_relative(math.floor(direction * 100 * sensitivity_x), math.floor(math.sin(current_time / 1000) * 100 * sensitivity_y), Delay.MINI) -- 视角运动：水平方向匀速运动，竖直方向简谐运动
                local duration = Runtime:get_running_time() - start_time
                if (not first_throw and 3000 < duration and duration < 6000)
                then
                    Keyboard:click(Keyboard.R, Delay.SHORT)
                    first_throw = true
                end
                if (not second_throw and 6000 < duration)
                then
                    Keyboard:click(Keyboard.R, Delay.SHORT)
                    second_throw = true
                end
            until (Runtime:get_running_time() - start_time > 7000)
            Mouse:release(Mouse.RIGHT) -- 松开鼠标右键释放旋风
        end
    },
    Weapon:new{
        name = "【擎空】突击套装",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.ONE },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【天驱】突击套装",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.TWO },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "冰雪魔咒PB-4",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.THREE },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "次元裁决",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.FOUR },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【青鸾】迅风轮",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.EIGHT },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "晖耀审判",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.FIVE },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【黯火】加特林",
        switch_delay = Delay.SHORT,
        number = Weapon.PRIMARY,
        purchase_sequence = { Keyboard.B, Keyboard.FIVE, Keyboard.SEVEN },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【黯影】Hecate II",
        switch_delay = Delay.SHORT,
        number = Weapon.PRIMARY,
        purchase_sequence = { Keyboard.B, Keyboard.FOUR, Keyboard.FOUR },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【恶龙】霰弹炮",
        switch_delay = Delay.NORMAL,
        number = Weapon.PRIMARY,
        purchase_sequence = { Keyboard.B, Keyboard.TWO, Keyboard.ONE },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【异噬】暗雷兽",
        switch_delay = Delay.NORMAL,
        number = Weapon.PRIMARY,
        purchase_sequence = { Keyboard.B, Keyboard.TWO, Keyboard.TWO },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "暗狱雷魂",
        switch_delay = Delay.NORMAL,
        number = Weapon.PRIMARY,
        purchase_sequence = { Keyboard.B, Keyboard.J, Keyboard.EIGHT, Keyboard.ONE },
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "逆界星轮",
        switch_delay = Delay.NORMAL,
        number = Weapon.PRIMARY,
        purchase_sequence = { Keyboard.B, Keyboard.EIGHT, Keyboard.SIX },
        attack_button = Mouse.RIGHT
    }
}

---特殊武器。
---@type Weapon
SpecialWeapon =
    ---特殊武器圣翼皓印（或炽翼魔印）
    Weapon:new{
        name = "圣翼皓印/炽翼魔印",
        switch_delay = 750,
        number = Weapon.GRENADE ,
        purchase_sequence = { Keyboard.B, Keyboard.EIGHT, Keyboard.NINE },
        discharging = false, -- 是否在释放光印
        discharge_start_moment = 0, --  光印释放的时刻。
        recharge_start_moment = 0, -- 充能开始的时刻。
        DISCHARGE_TIME = 25, -- 光印释放时间。
        RECHARGE_TIME = 10, -- 充能时间。
        ---为该武器重写 `use` 方法。
        ---@param self Weapon
        use = function (self)
            local current_time = DateTime:get_local_timestamp() -- 当前时间戳
            -- 当前正在充能，且充能时间超过 `RECHARGE_TIME`。
            if (not self.discharging and current_time - self.recharge_start_moment > self.RECHARGE_TIME)
            then
                self:switch()
                Mouse:click(Mouse.LEFT, 200)
                self.discharging = true -- 释放状态
                self.discharge_start_moment = current_time
            -- 当前正在释放，且释放时间超过 `DISCHARGE_TIME`。
            elseif (self.discharging and current_time - self.discharge_start_moment > self.DISCHARGE_TIME)
            then
                self:switch()
                -- 按 `Keyboard.R` 召唤界徐圣。 
                Mouse:move_relative(0, 4000, Delay.NORMAL)
                Keyboard:click(Keyboard.R, 350)
                Mouse:move_relative(0, -4000, Delay.NORMAL)
                self.discharging = false -- 充能状态
                self.recharge_start_moment = current_time
            end
        end
    }

----SpecialWeapon = nil --[[如果没有特殊武器，只需要将本行最前面的四条短横线删掉即可]]

---- v1.3 版本新增，只将下面的内容根据自身情况复制追加到上一版本 WeaponList.lua 中即可

---防弹衣
AC = Weapon:new({
    name = "防弹衣+头盔",
    purchase_sequence = { Keyboard.B, Keyboard.EIGHT, Keyboard.TWO }
})

----AC = nil --[[如果不需要购买护甲，则将本行最前面四条短横线删掉即可]]

end -- WeaponList_lua