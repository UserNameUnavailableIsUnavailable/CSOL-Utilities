if not WeaponList_lua then
    WeaponList_lua = true
    Include("Version.lua")
    Version:set("WeaponList", "1.5.3")
    Version:require("WeaponList", "Setting", "1.5.3")
    Version:require("WeaponList", "Weapon", "1.5.3")
    Armor =
        Weapon:new(
        {
            name = "防弹衣+头盔",
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
                name = "FNP-45 战损版",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.J,
                    Keyboard.ONE,
                    Keyboard.TWO
                }
            }
        )
    }

    DefaultConventionalWeapons = {
        Weapon:new(
            {
                name = "光棱剑",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.G
                },
                number = Weapon.MELEE,
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "甜筒剑",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.J,
                    Keyboard.NINE,
                    Keyboard.THREE
                },
                number = Weapon.MELEE,
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        )
    }

    DefaultSpecialWeapons = {
        Weapon:new(
            {
                name = "夙狼夜契",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.EIGHT,
                    Keyboard.NINE
                },
                template_name = "Lycaon",
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                switch_delay = 600,
                number = Weapon.GRENADE,
                last_phantom_rush_timepoint = 0,
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

    ExtendedPartWeapons = {}

    ExtendedConventionalWeapons = {
        Weapon:new(
            {
                name = "万钧神威",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.NINE,
                    Keyboard.NINE
                },
                template_name = "Brionac",
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                switch_delay = Delay.LONG,
                number = Weapon.MELEE,
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
                name = "逆界星轮",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.EIGHT,
                    Keyboard.FIVE
                },
                number = Weapon.PRIMARY,
                switch_delay = 100,
                attack_button = Mouse.RIGHT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "oscillating",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "青鸾",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.ONE,
                    Keyboard.FOUR
                },
                number = Weapon.SECONDARY,
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "甜筒剑",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.J,
                    Keyboard.NINE,
                    Keyboard.THREE
                },
                number = Weapon.MELEE,
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        ),
        Weapon:new(
            {
                name = "龙",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.EIGHT,
                    Keyboard.SEVEN
                },
                number = Weapon.PRIMARY,
                switch_delay = 100,
                attack_button = Mouse.LEFT,
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                attack_duration = 10
            }
        )
    }

    ExtendedSpecialWeapons = {
        Weapon:new(
            {
                name = "夙狼夜契",
                purchase_sequence = {
                    Keyboard.B,
                    Keyboard.EIGHT,
                    Keyboard.NINE
                },
                template_name = "Lycaon",
                horizontal_strafe_mode = "random",
                vertical_strafe_mode = "none",
                switch_delay = 600,
                number = Weapon.GRENADE,
                last_phantom_rush_timepoint = 0,
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
