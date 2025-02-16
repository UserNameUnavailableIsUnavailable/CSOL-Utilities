if not WeaponList_lua
then
	WeaponList_lua = true
	Armor = Weapon:new{
		name = "防弹衣+头盔",
		purchase_sequence = { Keyboard.B, Keyboard.EIGHT, Keyboard.TWO }
	}
	DefaultPartWeapons = {
		Weapon:new{
			name = "星战前线·加特林",
			number = Weapon.NULL,
			purchase_sequence = { Keyboard.B, Keyboard.J, Keyboard.TWO, Keyboard.FOUR }
		},
		Weapon:new{
			name = "FNP-45战损版",
			number = Weapon.NULL,
			purchase_sequence = { Keyboard.B, Keyboard.J, Keyboard.ONE, Keyboard.TWO }
		},
		Weapon:new{
			name = "燃爆Ignite-10",
			number = Weapon.NULL,
			purchase_sequence = { Keyboard.B, Keyboard.J, Keyboard.EIGHT, Keyboard.SEVEN }
		}
	}
	DefaultConventionalWeapons = {
		Weapon:new{
			name = "幻境！光棱剑",
			number = Weapon.MELEE,
			purchase_sequence = { Keyboard.B, Keyboard.G },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		}
	}
	DefaultSpecialWeapons = {

	}
	ExtendedPartWeapons = {

	}
	ExtendedConventionalWeapons = {
		Weapon:new{
			name = "神鬼开天/魔神开天",
			switch_delay = Delay.SHORT,
			number = Weapon.MELEE,
			purchase_sequence = {
				Keyboard.B,
				Keyboard.NINE,
				Keyboard.SIX
			},
			attack = function (self)
				Mouse:press(Mouse.RIGHT)
				local sensitivity_x = 1 - 0.8 * math.random()
				local sensitivity_y = 1 - 0.8 * math.random()
				local direction = Utility:random_direction()
				local start_time = Runtime:get_running_time()
				local last_switch_time = 0
				repeat					local current_time = Runtime:get_running_time()
					if current_time - last_switch_time > 1000
					then
						self:switch_without_delay()
						last_switch_time = current_time
					end
					if Mouse:move_relative(math.floor(direction * 100 * sensitivity_x), math.floor(math.sin(current_time / 1000) * 100 * sensitivity_y), Delay.MINI)
					then
						break
					end
				until Runtime:get_running_time() - start_time > 6000
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
			purchase_sequence = {
				Keyboard.B,
				Keyboard.NINE,
				Keyboard.FOUR
			},
			attack = function (self)
				Mouse:press(Mouse.RIGHT)
				local sensitivity_x = 1 - 0.8 * math.random()
				local sensitivity_y = 1 - 0.8 * math.random()
				local direction = Utility:random_direction()
				local start_time = Runtime:get_running_time()
				local last_switch_time = 0
				local first_throw = false
				local second_throw = false
				repeat					local current_time = Runtime:get_running_time()
					if current_time - last_switch_time > 1000
					then
						self:switch_without_delay()
						last_switch_time = current_time
					end
					if not Mouse:move_relative(math.floor(direction * 100 * sensitivity_x), math.floor(math.sin(current_time / 1000) * 100 * sensitivity_y), Delay.MINI)
					then
						break
					end
					local duration = Runtime:get_running_time() - start_time
					if not first_throw and 3000 < duration and duration < 6000
					then
						Keyboard:click(Keyboard.R, Delay.SHORT)
						first_throw = true
					end
					if not second_throw and 6000 < duration
					then
						Keyboard:click(Keyboard.R, Delay.SHORT)
						second_throw = true
					end
				until Runtime:get_running_time() - start_time > 7000
				return Mouse:release(Mouse.RIGHT)
			end
		},
		Weapon:new{
			name = "【擎空】突击套装",
			number = Weapon.SECONDARY,
			purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.ONE },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "【天驱】突击套装",
			number = Weapon.SECONDARY,
			purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.TWO },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "冰雪魔咒PB-4",
			number = Weapon.SECONDARY,
			purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.THREE },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "次元裁决",
			number = Weapon.SECONDARY,
			purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.FOUR },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "【青鸾】迅风轮",
			number = Weapon.SECONDARY,
			purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.EIGHT },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "晖耀审判",
			number = Weapon.SECONDARY,
			purchase_sequence = { Keyboard.B, Keyboard.ONE, Keyboard.FIVE },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "【黯火】加特林",
			number = Weapon.PRIMARY,
			purchase_sequence = { Keyboard.B, Keyboard.FIVE, Keyboard.SEVEN },
			switch_delay = Delay.SHORT,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "【黯影】Hecate II",
			number = Weapon.PRIMARY,
			purchase_sequence = { Keyboard.B, Keyboard.FOUR, Keyboard.FOUR },
			switch_delay = Delay.SHORT,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "【恶龙】霰弹炮",
			number = Weapon.PRIMARY,
			purchase_sequence = { Keyboard.B, Keyboard.TWO, Keyboard.ONE },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "【异噬】暗雷兽",
			number = Weapon.PRIMARY,
			purchase_sequence = { Keyboard.B, Keyboard.TWO, Keyboard.TWO },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "暗狱雷魂",
			number = Weapon.PRIMARY,
			purchase_sequence = { Keyboard.B, Keyboard.J, Keyboard.EIGHT, Keyboard.ONE },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.LEFT,
		},
		Weapon:new{
			name = "逆界星轮",
			number = Weapon.PRIMARY,
			purchase_sequence = { Keyboard.B, Keyboard.EIGHT, Keyboard.SIX },
			switch_delay = Delay.NORMAL,
			attack_button = Mouse.RIGHT,
		}
	}
	ExtendedSpecialWeapons = {
		Weapon:new{
			name = "圣翼皓印/炽翼魔印",
			switch_delay = 750,
			number = Weapon.GRENADE,
			purchase_sequence = {
				Keyboard.B,
				Keyboard.EIGHT,
				Keyboard.NINE
			},
			discharging = false,
			discharge_start_moment = 0,
			recharge_start_moment = 0,
			DISCHARGE_TIME = 25,
			RECHARGE_TIME = 10,
			use = function (self)
				local current_time = DateTime:get_local_timestamp()
				if not self.discharging and current_time - self.recharge_start_moment > self.RECHARGE_TIME
				then
					self:switch()
					Mouse:click(Mouse.LEFT, 200)
					self.discharging = true
					self.discharge_start_moment = current_time
				elseif self.discharging and current_time - self.discharge_start_moment > self.DISCHARGE_TIME
				then
					self:switch()
					Mouse:move_relative(0, 4000, Delay.NORMAL)
					Keyboard:click(Keyboard.R, 350)
					Mouse:move_relative(0, - 4000, Delay.NORMAL)
					self.discharging = false
					self.recharge_start_moment = current_time
				end
			end
		}
	}
end