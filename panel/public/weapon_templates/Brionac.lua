---万钧神威（Brionac）模板
---@class Brionac : Weapon
---@field last_throw_time integer 上一次投掷的时间点（毫秒）
Weapon:new({
    name = "万钧神威",
    switch_delay = Delay.LONG,
    number = Weapon.MELEE,
    purchase_sequence = {},
    template_name = "Brionac",
    attack_duration = 20,
    last_throw_time = 0,
    ---重写 attack 方法，按照下面定义的方式进行攻击。
    ---若您需要使用万钧神威进行挂机且没有编程经验，则请勿修改此函数。
    ---@param self Brionac
    ---@param round integer
    ---@param begin_timepoint integer
    ---@return function
    fire_interator = function(self, round, begin_timepoint)
        if round == 0 then
            self.last_throw_time = begin_timepoint
            return function() Mouse:press(Mouse.RIGHT) end
        end
        if round > 0 then
            return function()
                local tp = Runtime:get_running_time()
                -- 每隔 4 秒投掷一次
                if tp - self.last_throw_time > 4000 then
                    Keyboard:click(Weapon.reload_key, 0)
                    self.last_throw_time = tp
                end
            end
        end
        return function() Mouse:release(Mouse.RIGHT) end
    end
})
