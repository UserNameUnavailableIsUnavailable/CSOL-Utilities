if not WeaponList_lua then
    WeaponList_lua = true
    Include("Version.lua")
    Version:set("WeaponList", "1.5.3")
    Version:require("WeaponList", "Setting", "1.5.3")
    Version:require("WeaponList", "Weapon", "1.5.3")

    Armor =
        Weapon:new(
        {
            name = "防弹衣 + 头盔",
            purchase_sequence = {
                Keyboard.B,
                Keyboard.EIGHT,
                Keyboard.TWO
            }
        }
    )

    DefaultPartWeapons = {
        Weapon:new(
            {
                name = "星战前线·加特林",
                number = Weapon.NULL,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.J,
                    Keyboard.TWO,
                    Keyboard.FOUR
                }
            }
        ),
        Weapon:new(
            {
                name = "FNP-45 战损版",
                number = Weapon.NULL,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.J,
                    Keyboard.ONE,
                    Keyboard.TWO
                }
            }
        ),
        Weapon:new(
            {
                name = "燃爆 Ignite-10",
                number = Weapon.NULL,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.J,
                    Keyboard.EIGHT,
                    Keyboard.SEVEN
                }
            }
        )
    }

    DefaultConventionalWeapons = {
        Weapon:new(
            {
                name = "幻境！光棱剑",
                purchase_sequence = {Keyboard.B, Keyboard.G},
                number = Weapon.PRIMARY,
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        )
    }

    DefaultSpecialWeapons = {}

    ExtendedPartWeapons = {}

    ExtendedConventionalWeapons = {
        Weapon:new(
            {
                name = "魔神开天",
                switch_delay = Delay.LONG,
                number = Weapon.MELEE,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.NINE,
                    Keyboard.SIX
                },
                template_name = "TwinLightAxes",
                attack = function(self)
                    Mouse:press(Mouse.RIGHT)
                    local sensitivity_x = 1 - 0.8 * math.random()
                    local sensitivity_y = 1 - 0.8 * math.random()
                    local direction = Utility:random_direction()
                    local start_time = Runtime:get_running_time()
                    local last_switch_time = 0
                    repeat
                        local current_time = Runtime:get_running_time()
                        if current_time - last_switch_time > 1000 then
                            self:switch_without_delay()
                            last_switch_time = current_time
                        end
                        Mouse:move_relative(
                            math.ceil(direction * 100 * sensitivity_x / Setting.FIELD_IN_GAME_SENSITIVITY),
                            math.ceil(
                                math.sin(current_time / 1000) * 100 * sensitivity_y / Setting.FIELD_IN_GAME_SENSITIVITY
                            ),
                            Delay.MINI
                        )
                    until Runtime:get_running_time() - start_time > 6000
                    Mouse:release(Mouse.RIGHT, 200)
                    Mouse:press(Mouse.LEFT, 1000)
                    Keyboard:press(Weapon.reload_key, 200)
                    Keyboard:release(Weapon.reload_key)
                    Mouse:release(Mouse.LEFT)
                end,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                last_whirl_time = 0,
                fire_interator = function(self, round, begin_timepoint)
                    if round == 0 then
                        self.last_light_vortex_time = begin_timepoint
                        return function()
                            Mouse:press(Mouse.RIGHT)
                        end
                    end
                    if round > 0 then
                        return function()
                            local tp = Runtime:get_running_time()
                            if tp - self.last_light_vortex_time > 6000 then
                                Mouse:release(Mouse.RIGHT, 200)
                                Mouse:press(Mouse.LEFT, 1000)
                                Keyboard:press(Weapon.reload_key, 200)
                                Keyboard:release(Weapon.reload_key)
                                Mouse:release(Mouse.LEFT)
                                Mouse:press(Mouse.RIGHT)
                                self.last_light_vortex_time = Runtime:get_running_time()
                            end
                        end
                    end
                    if round < 0 then
                        return function()
                            Mouse:release(Mouse.RIGHT)
                        end
                    end
                end
            }
        ),
        Weapon:new(
            {
                name = "万钧神威",
                switch_delay = Delay.LONG,
                number = Weapon.MELEE,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.NINE,
                    Keyboard.FOUR
                },
                template_name = "Brionac",
                attack = function(self)
                    Mouse:press(Mouse.RIGHT)
                    local sensitivity_x = 1 - 0.8 * math.random()
                    local sensitivity_y = 1 - 0.8 * math.random()
                    local direction = Utility:random_direction()
                    local start_time = Runtime:get_running_time()
                    local last_throw_time = Runtime:get_running_time()
                    repeat
                        local current_time = Runtime:get_running_time()
                        Mouse:move_relative(
                            math.ceil(direction * 100 * sensitivity_x / Setting.FIELD_IN_GAME_SENSITIVITY),
                            math.ceil(
                                math.sin(current_time / 1000) * 100 * sensitivity_y / Setting.FIELD_IN_GAME_SENSITIVITY
                            ),
                            Delay.MINI
                        )
                        if Runtime:get_running_time() - last_throw_time > 4000 then
                            Keyboard:click(Weapon.reload_key, Delay.MINI)
                            last_throw_time = Runtime:get_running_time()
                        end
                    until Runtime:get_running_time() - start_time > 9000
                    Mouse:release(Mouse.RIGHT)
                end,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                last_throw_time = 0,
                fire_interator = function(self, round, begin_timepoint)
                    if round == 0 then
                        self.last_throw_time = begin_timepoint
                        return function()
                            Mouse:press(Mouse.RIGHT)
                        end
                    end
                    if round > 0 then
                        return function()
                            local tp = Runtime:get_running_time()
                            if tp - self.last_throw_time > 4000 then
                                Keyboard:click(Weapon.reload_key, 0)
                                self.last_throw_time = tp
                            end
                        end
                    end
                    if round < 0 then
                        return function()
                            Mouse:release(Mouse.RIGHT)
                        end
                    end
                end
            }
        ),
        Weapon:new(
            {
                name = "【擎空】突击套装",
                number = Weapon.SECONDARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.ONE,
                    Keyboard.ONE
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "【天驱】突击套装",
                number = Weapon.SECONDARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.ONE,
                    Keyboard.TWO
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "冰雪魔咒 PB-4",
                number = Weapon.SECONDARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.ONE,
                    Keyboard.THREE
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "次元裁决",
                number = Weapon.SECONDARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.ONE,
                    Keyboard.FOUR
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "【青鸾】迅风轮",
                number = Weapon.SECONDARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.ONE,
                    Keyboard.EIGHT
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "晖耀审判",
                number = Weapon.SECONDARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.ONE,
                    Keyboard.FIVE
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "【黯火】加特林",
                number = Weapon.PRIMARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.FIVE,
                    Keyboard.SEVEN
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "【黯影】Hecate II",
                number = Weapon.PRIMARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.FOUR,
                    Keyboard.FOUR
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "【恶龙】霰弹炮",
                number = Weapon.PRIMARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.TWO,
                    Keyboard.ONE
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "【异噬】暗雷兽",
                number = Weapon.PRIMARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.TWO,
                    Keyboard.TWO
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "暗狱雷魂",
                number = Weapon.PRIMARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.J,
                    Keyboard.EIGHT,
                    Keyboard.ONE
                },
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "逆界星轮",
                number = Weapon.PRIMARY,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.EIGHT,
                    Keyboard.SIX
                },
                switch_delay = 100,
                attack_button = Mouse.RIGHT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "oscillating",
                attack_duration = 7
            }
        )
    }

    ExtendedSpecialWeapons = {
        Weapon:new(
            {
                name = "夙狼夜契",
                switch_delay = 600,
                number = Weapon.GRENADE,
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.EIGHT,
                    Keyboard.NINE
                },
                template_name = "Lycaon",
                discharging = false,
                discharge_start_moment = 0,
                charge_start_moment = 0,
                DISCHARGE_TIME = 27,
                RECHARGE_TIME = 13,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                last_strike_timepoint = 0,
                garrison = false,
                cooldown_time = 30,
                attack = function(self)
                    local current_tp = DateTime:get_local_timestamp()
                    if current_tp - self.last_phantom_rush_timepoint > self.cooldown_time then
                        self:switch()
                        Mouse:click(Mouse.RIGHT, 500)
                        Keyboard:click(self.reload_key, 500)
                        self.last_phantom_rush_timepoint = current_tp
                    end
                end
            }
        )
    }
end
