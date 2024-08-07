if (not Executor_lua)
then
Executor_lua = true
Executor = {}

---注册中断处理函数。必须在 `Keyboard` 加载之后执行。
Runtime:register_interrupt_handler(function (self)
    if (not self.interrupt_flag) -- 未关中断才会触发中断
    then
        return
    end
    -- 中断开始时，中断标志位使能以屏蔽后续中断
    self.interrupt_flag = false -- 关中断
    -- 现场切换，保护中断现场
    -- 是否暂停执行
    if (Keyboard:is_modifier_pressed(Keyboard.LCTRL) and Keyboard:is_modifier_pressed(Keyboard.RCTRL))
    then
        Keyboard:release_all()
        Mouse:release_all()
        -- 注意，如果 pause_flag == true，则 restore_context() 不会再恢复中断现场，这是由于 pause_flag 置位后不会执行任何键鼠操作
        if (not Runtime.manual_flag)
        then
            Console:infomation("开始手动接管，禁用键鼠动作。")
        end
        Runtime.manual_flag = true -- 暂停执行，中断现场将不会恢复
    elseif (Keyboard:is_modifier_pressed(Keyboard.LALT) and Keyboard:is_modifier_pressed(Keyboard.RALT))
    then
        if (Runtime.manual_flag)
        then
            Console:infomation("中止手动接管，允许键鼠动作。")
        end
        Runtime.manual_flag = false -- 恢复执行，后续中断现场可正常恢复
    end
    -- 中断处理完毕
    self.interrupt_flag = true -- 开中断
end)

---创建游戏房间。
function Executor:create_game_room()
    Keyboard:click_several_times(Keyboard.ESCAPE, 10, 500) -- 按 10 次 `Keyboard.ESCAPE`，关闭所有弹窗
    Mouse:click_on_several_times(Setting.HALL_BACK_X, Setting.HALL_BACK_Y, 10, 100) -- 按 10 次返回，到大厅进入游戏界面
    Keyboard:click_several_times(Keyboard.ESCAPE, 10, 500) -- 按 10 次 `Keyboard.ESCAPE`，关闭所有弹窗
    Mouse:click_on(Setting.HALL_ROOM_LIST_X, Setting.HALL_ROOM_LIST_Y, 2000)
    Mouse:click_on(Setting.HALL_CREATE_ROOM_X, Setting.HALL_CREATE_ROOM_Y, 2000)
    Mouse:click_on(Setting.GAME_MODE_X, Setting.GAME_MODE_Y, 2000)
    Mouse:click_on(Setting.ZOMBIE_SCENARIO_MODE_X, Setting.ZOMBIE_SCENARIO_MODE_Y, 2000)
    Mouse:click_on_several_times(Setting.MAP_CHOOSE_LEFT_SCROLL_X, Setting.MAP_CHOOSE_LEFT_SCROLL_Y, 20, 2000)
    Mouse:click_on(Setting.MAP_TRAP_X, Setting.MAP_TRAP_Y, 2000)
    Mouse:click_on(Setting.FINISH_CHOOSE_X, Setting.FINISH_CHOOSE_Y, 2000)
    Mouse:click_on(Setting.GAME_DIFFICULTY_X, Setting.GAME_DIFFICULTY_Y, 2000)
    Mouse:click_on(Setting.GAME_DIFFICULTY_OPTION_X, Setting.GAME_DIFFICULTY_OPTION_Y, 2000)
    Mouse:click_on(Setting.CREATE_ROOM_X, Setting.CREATE_ROOM_Y, 5000)
end

---点击“开始游戏”按钮，开始游戏。
function Executor:start_game_room()
    Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI)
    Mouse:click_on(Setting.ROOM_START_GAME_X, Setting.ROOM_START_GAME_Y, 2000)
end

---选定角色。
---@return nil
function Executor:choose_class()
    if (Setting.CHOOSE_T_CLASS)
    then
        Mouse:click_on(Setting.CHOOSE_T_CLASS_X, Setting.CHOOSE_T_CLASS_Y, 500)
    end
    if (Setting.CLASS_OPTION and math.type(Setting.CLASS_OPTION) == "integer" and Setting.CLASS_OPTION >= 0 and Setting.CLASS_OPTION <= 9)
    then
        Keyboard:click(tostring(Setting.CLASS_OPTION), Delay.NORMAL)
    else
        Console:information("Setting.CLASS_OPTION 设置有误。随机选择角色。")
        Keyboard:click(Keyboard.ZERO, Delay.NORMAL)
    end
    Runtime:sleep(500) -- 等待 0.5 秒，足够控制器更新下条命令
end

---上一次尝试确认结算界面的时间戳。
Executor.last_confirm_timestamp = 0
function Executor:try_confirm()
    local current_timestamp = DateTime:get_local_timestamp()
    if (math.abs(current_timestamp - self.last_confirm_timestamp) < 20) -- 未超过 20 秒
    then
        return
    end
    Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI)
    Mouse:click_on(Setting.GAME_ROUND_CONFIRM_X, Setting.GAME_ROUND_CONFIRM_Y)
    self.last_confirm_timestamp = current_timestamp
end

---合成配件。
function Executor:combine_parts()
    local counter = 20
    Keyboard:press(Keyboard.ENTER, 10)
    repeat
        Mouse:click_on(
            Setting.CRAFT_PARTS_AUTO_FILL_X,
            Setting.CRAFT_PARTS_AUTO_FILL_Y,
            10
        )
        Mouse:click_on(
            Setting.CRAFT_PARTS_COMBINE_X,
            Setting.CRAFT_PARTS_COMBINE_Y,
            10
        )
        counter = counter - 1
    until (counter == 0)
    Keyboard:release(Keyboard.ENTER, 10)
    Mouse:click_on(
        Setting.CRAFT_PARTS_CLEAR_X,
        Setting.CRAFT_PARTS_CLEAR_Y,
        20
    )
end

Executor.buy_button_x = 0
Executor.buy_button_y = 0
---购买商店物品。
---@param buy_button_x integer|nil 横坐标。
---@param buy_button_y integer|nil 纵坐标。
function Executor:purchase_item(buy_button_x, buy_button_y)
    self.buy_button_x = buy_button_x or self.buy_button_x or 0
    self.buy_button_y = buy_button_y or self.buy_button_y or 0
    Mouse:click_on(Executor.buy_button_x, Executor.buy_button_y, 50)
    Mouse:click_on(Setting.STORE_BUY_OPTION_X, Setting.STORE_BUY_OPTION_Y, 50) -- 弹出界面选项
    Mouse:click_on(Setting.STORE_BUY_X, Setting.STORE_BUY_Y, 50) -- 弹出界面兑换按钮
    Mouse:click_on(Setting.STORE_BUY_CONFIRM_X, Setting.STORE_BUY_CONFIRM_Y, 100) -- 兑换后确认
    Keyboard:click(Keyboard.ESCAPE, 50)
end

---上一次定位得到的光标
Executor.last_location_x = -1
Executor.last_location_y = -1
---光标定位。
function Executor:locate_cursor()
    if (Keyboard:is_modifier_pressed(Keyboard.CTRL) and Keyboard:is_modifier_pressed(Keyboard.ALT))
    then
        local x, y = Mouse:locate_cursor()
        if (x ~= self.last_location_x and y ~= self.last_location_y)
        then
            Console:infomation("光标位置：(%d, %d)", x, y)
        end
        Runtime:sleep(500)
    end
end

end -- Executor_lua