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
        purchase_sequence = {Keyboard.B, Keyboard.J, Keyboard.TWO, Keyboard.FOUR}
    }),
    Weapon:new({
        name = "FNP-45战损版",
        number = Weapon.SECONDARY,
        purchase_sequence = {Keyboard.B, Keyboard.J, Keyboard.ONE, Keyboard.TWO}
    }),
    Weapon:new({
        name = "风暴之锤",
        number = Weapon.MELEE,
    }),
    Weapon:new({
        name = "燃爆Ignite-10",
        number = Weapon.GRENADE,
        purchase_sequence = {Keyboard.B, Keyboard.J, Keyboard.EIGHT, Keyboard.SEVEN}
    })
}

---默认武器列表。
---@type Weapon[]
DefaultWeaponList = {
    Weapon:new{
        name = "幻境！光棱剑",
        switch_delay = Delay.NORMAL,
        number = Weapon.MELEE,
        purchase_sequence = {Keyboard.B, Keyboard.G},
        attack_button = Mouse.LEFT
    }
}

---扩展武器列表。
---@type Weapon[]
ExtendedWeaponList = {
    Weapon:new{
        name = "幻境！光棱剑",
        switch_delay = Delay.NORMAL,
        number = Weapon.MELEE,
        purchase_sequence = {Keyboard.B, Keyboard.G},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【擎空】突击套装",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = {Keyboard.B, Keyboard.ONE, Keyboard.ONE},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【天驱】突击套装",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = {Keyboard.B, Keyboard.ONE, Keyboard.TWO},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "冰雪魔咒PB-4",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = {Keyboard.B, Keyboard.ONE, Keyboard.THREE},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "次元裁决",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = {Keyboard.B, Keyboard.ONE, Keyboard.FOUR},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "晖耀审判",
        switch_delay = Delay.NORMAL,
        number = Weapon.SECONDARY,
        purchase_sequence = {Keyboard.B, Keyboard.ONE, Keyboard.FIVE},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【黯火】加特林",
        switch_delay = Delay.SHORT,
        number = Weapon.PRIMARY,
        purchase_sequence = {Keyboard.B, Keyboard.FIVE, Keyboard.SEVEN},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【黯影】Hecate II",
        switch_delay = Delay.SHORT,
        number = Weapon.PRIMARY,
        purchase_sequence = {Keyboard.B, Keyboard.FOUR, Keyboard.FOUR},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【恶龙】霰弹炮",
        switch_delay = Delay.NORMAL,
        number = Weapon.PRIMARY,
        purchase_sequence = {Keyboard.B, Keyboard.TWO, Keyboard.ONE},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "【异噬】暗雷兽",
        switch_delay = Delay.NORMAL,
        number = Weapon.PRIMARY,
        purchase_sequence = {Keyboard.B, Keyboard.TWO, Keyboard.TWO},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "暗狱雷魂",
        switch_delay = Delay.NORMAL,
        number = Weapon.PRIMARY,
        purchase_sequence = {Keyboard.B, Keyboard.J, Keyboard.EIGHT, Keyboard.ONE},
        attack_button = Mouse.LEFT
    },
    Weapon:new{
        name = "逆界星轮",
        switch_delay = Delay.NORMAL,
        number = Weapon.PRIMARY,
        purchase_sequence = {Keyboard.B, Keyboard.EIGHT, Keyboard.SIX},
        attack_button = Mouse.RIGHT
    }
}

---特殊武器。
---@type Weapon
SpecialWeapon =
    Weapon:new({
        name = "圣翼皓印",
        switch_delay = Delay.LONG_LONG,
        number = Weapon.GRENADE ,
        purchase_sequence = {Keyboard.B, Keyboard.EIGHT, Keyboard.NINE},
        discharging = false, -- 是否在释放光印
        discharge_start_moment = 0, --  光印释放的时刻。
        charge_start_moment = 0, -- 充能开始的时刻。
        DISCHARGE_TIME = 25, -- 光印释放时间。
        RECHARGE_TIME = 10, -- 充能时间。
        use = function (self)
            local current_time = DateTime:get_local_timestamp() -- 当前时间戳
            -- 当前正在充能，且充能时间超过 `RECHARGE_TIME`。
            if (not self.discharging and current_time - self.charge_start_moment > self.RECHARGE_TIME)
            then
                self.discharging = true
                self.discharge_start_moment = current_time
                self:switch()
                Mouse:click(Mouse.LEFT, 200)
            -- 当前正在释放，且释放时间超过 `DISCHARGE_TIME`。
            elseif (self.discharging and current_time - self.charge_start_moment > self.DISCHARGE_TIME)
            then
                self.discharging = false
                self.charge_start_moment = current_time
                self:switch()
                -- 按 `Keyboard.R` 召唤界徐圣。 
                Mouse:move_relative(0, 4000, Delay.NORMAL)
                Keyboard:click(Keyboard.R, 350)
                Mouse:move_relative(0, -4000, Delay.NORMAL)
            end
        end
    })

----SpecialWeapon = nil --[[如果没有特殊武器，只需要将本行最前面的四条短横线删掉即可]]

---- v1.3 版本新增，只将下面的内容根据自身情况复制追加到上一版本 WeaponList.lua 中即可

---防弹衣
AC = Weapon:new({
    name = "防弹衣",
    purchase_sequence = {Keyboard.B, Keyboard.EIGHT, Keyboard.TWO}
})


end -- WeaponList_lua