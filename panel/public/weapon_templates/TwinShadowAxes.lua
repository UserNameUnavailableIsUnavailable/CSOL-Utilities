---神鬼开天（Twin Shadow Axes）模板
---@class TwinShadowAxes : Weapon
---@field last_shadow_vortex_time integer 上一次释放大回旋的时刻
Weapon:new({
    name = "神鬼开天",
    switch_delay = Delay.LONG,
    number = Weapon.MELEE,
    purchase_sequence = {},
    last_shadow_vortex_time = 0,
    template_name = "TwinShadowAxes",
    ---@param self TwinShadowAxes
    ---@param round integer
    ---@param begin_timepoint integer
    fire_interator = function(self, round, begin_timepoint)
        if round == 0 then
            self.last_shadow_vortex_time = begin_timepoint
            return function() Mouse:press(Mouse.RIGHT) end -- 按下攻击按钮进行范围攻击
        end
        if round > 0 then
            return function()
                local tp = Runtime:get_running_time()
                -- 每隔 6 秒进行一次特殊攻击
                if tp - self.last_shadow_vortex_time > 6000 then
                    Mouse:release(Mouse.RIGHT, 200) -- 松开右键
                    Mouse:press(Mouse.LEFT, 1000) -- 按下左键触发竖劈
                    Keyboard:press(Weapon.reload_key, 200) -- 按下左键的同时再按下 R 键触发大回旋
                    Keyboard:release(Weapon.reload_key)
                    Mouse:release(Mouse.LEFT) -- 松开左键
                    Mouse:press(Mouse.RIGHT) -- 重新按下右键
                    self.last_shadow_vortex_time = Runtime:get_running_time()
                end
            end
        end
        if round < 0 then
            return function() Mouse:release(Mouse.RIGHT) end
        end
    end
})
