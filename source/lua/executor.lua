if (not Executor_lua)
then
Executor_lua = true
Executor = {}

---创建游戏房间。
function Executor:create_game_room()
    Keyboard:click_several_times(Keyboard.ESCAPE, 10, 500) -- 按 10 次 `Keyboard.ESCAPE`，关闭所有弹窗
    Mouse:click_on(Setting.ROOM_LIST_X, Setting.ROOM_LIST_Y, 1000)
    Mouse:click_on(Setting.HALL_CREATE_ROOM_X, Setting.HALL_CREATE_ROOM_Y, 1000)
    Mouse:click_on(Setting.GAME_MODE_X, Setting.GAME_MODE_Y, 1000)
    Mouse:click_on(Setting.ZOMBIE_SCENARIO_MODE_X, Setting.ZOMBIE_SCENARIO_MODE_Y, 1000)
    Mouse:click_on_several_times(Setting.MAP_CHOOSE_LEFT_SCROLL_X, Setting.MAP_CHOOSE_LEFT_SCROLL_Y, 10, 1000)
    Mouse:click_on(Setting.MAP_TRAP_X, Setting.MAP_TRAP_Y, 1000)
    Mouse:click_on(Setting.FINISH_CHOOSE_X, Setting.FINISH_CHOOSE_Y, 1000)
    Mouse:click_on(Setting.GAME_DIFFICULTY_X, Setting.GAME_DIFFICULTY_Y, 1000)
    Mouse:click_on(Setting.GAME_DIFFICULTY_OPTION_X, Setting.GAME_DIFFICULTY_OPTION_Y, 1000)
    Mouse:click_on(Setting.CREATE_ROOM_X, Setting.CREATE_ROOM_Y, 1000)
    Runtime:sleep(5000)
end

---点击“开始游戏”按钮，开始游戏。
function Executor:start_game_room()
    Keyboard:click_several_times(Keyboard.ESCAPE, 2, Delay.NORMAL)
    Mouse:click_on(Setting.GAMESTART_X, Setting.GAMESTART_Y)
    Console:println("Start Game Room")
    Runtime:sleep(2000) -- 等待两秒，足够控制器更新下条命令
end

---选定角色。
---@param is_terrorist boolean | nil 是否需要选择恐怖分子阵营。
---@param choice integer | string 角色选项，可取 `"0" ~ "9"`。
---@return nil
function Executor:choose_class(is_terrorist, choice)
    if (is_terrorist)
    then
        Mouse:click_on(Setting.CHOOSE_T_CLASS_X, Setting.CHOOSE_T_CLASS_Y)
    end
    Keyboard:click(tostring(choice))
    Runtime:sleep(2000) -- 等待两秒，足够控制器更新下条命令
end

---上一次尝试确认结算界面的时间戳。
Executor.last_confirm_timestamp = 0
function Executor:try_confirm()
    local current_timestamp = DateTime:get_local_timestamp()
    if (math.abs(current_timestamp - self.last_confirm_timestamp) < 20) -- 未超过 20 秒
    then
        return
    end
    Keyboard:click_several_times(Keyboard.ESCAPE, 4)
    Mouse:click_on(Setting.GAME_SETTLEMENT_CONFIRM_X, Setting.GAME_SETTLEMENT_CONFIRM_Y)
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
        10
    )
end

---购买商店物品。
function Executor:purchase_item(buy_button_x, buy_button_y)
    Mouse:click_on(buy_button_x, buy_button_y, 20)
    Mouse:click_on(Setting.STORE_BUY_OPTION_X, Setting.STORE_BUY_OPTION_Y, 20) -- 弹出界面选项
    Mouse:click_on(Setting.STORE_BUY_X, Setting.STORE_BUY_Y, 20) -- 弹出界面兑换按钮
    Mouse:click_on(Setting.STORE_BUY_CONFIRM_X, Setting.STORE_BUY_CONFIRM_Y, 100) -- 兑换后确认
    Keyboard:click(Keyboard.ESCAPE, 20)
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