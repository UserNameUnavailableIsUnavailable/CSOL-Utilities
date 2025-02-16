if (not Executor_lua)
then
Executor_lua = true
Executor = {}

Executor.command_in_execution = Command.CMD_NOP

---注册暂停事件处理函数，处理用户手动接管事件。
Runtime:register_interrupt_handler(
    function ()
        if (Keyboard:is_modifier_pressed(Keyboard.LCTRL) and Keyboard:is_modifier_pressed(Keyboard.RCTRL))
        then
            Keyboard:release_all()
            Mouse:release_all()
            -- 注意，如果 pause_flag == true，则 restore_context() 不会再恢复中断现场，这是由于 pause_flag 置位后不会执行任何键鼠操作
            if (not Runtime.manual_flag)
            then
                Console:information("开始手动接管，禁用键鼠动作。")
            end
            Runtime.manual_flag = true -- 暂停执行，中断现场将不会恢复
        elseif (Keyboard:is_modifier_pressed(Keyboard.LALT) and Keyboard:is_modifier_pressed(Keyboard.RALT))
        then
            if (Runtime.manual_flag)
            then
                Console:information("中止手动接管，允许键鼠动作。")
            end
            Runtime.manual_flag = false -- 恢复执行，后续中断现场可正常恢复
        end
    end
)

Executor.last_reset_or_respawn_time = 0
---注册回合重置检查函数，游戏开始后每隔 8 秒进行回合重置。
Runtime:register_interrupt_handler(
    function ()
        -- 当前未在挂机
        if (Executor.command_in_execution ~= Command.CMD_PLAY_GAME_NORMAL and Executor.command_in_execution ~= Command.CMD_PLAY_GAME_EXTEND)
        then
            return
        end
        -- 当前正在操作，则等到操作完全结束再重置，防止对操作逻辑造成不必要的干扰
        if (Player:is_playing())
        then
            return
        end
        local time= Runtime:get_running_time()
        if (time - Executor.last_reset_or_respawn_time > 8000)
        then
            Player:reset_round_or_respawn()
            Executor.last_reset_or_respawn_time = time
        end
    end
)

---创建游戏房间。
function Executor:create_game_room()
    if (not Setting.SWITCH_CREATE_ROOM_ON_EXCEPTION)
    then
        return
    end
    Keyboard:click_several_times(Keyboard.ESCAPE, 10, 100) -- 按 10 次 `Keyboard.ESCAPE`，关闭所有弹窗
    Mouse:click_on_several_times(Setting.POSITION_LOBBY_BACK_X, Setting.POSITION_LOBBY_BACK_Y, 5, 100) -- 按 5 次返回，到大厅进入游戏界面
    Keyboard:click_several_times(Keyboard.ESCAPE, 10, 100) -- 按 10 次 `Keyboard.ESCAPE`，关闭所有弹窗
    Mouse:click_on(Setting.POSITION_LOBBY_LIST_ROOMS_X, Setting.POSITION_LOBBY_LIST_ROOMS_Y, 500)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_1_X, Setting.POSITION_LOBBY_CREATE_ROOM_1_Y, 500)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_GAME_MODE_1_X, Setting.POSITION_LOBBY_CREATE_ROOM_GAME_MODE_1_Y, 500)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_GAME_MODE_2_X, Setting.POSITION_LOBBY_CREATE_ROOM_GAME_MODE_2_Y, 500)
    Mouse:click_on_several_times(Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_LEFT_SCROLL_X, Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_LEFT_SCROLL_Y, 20, 100)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_MAP_OPTION_X, Setting.POSITION_LOBBY_CREATE_ROOM_MAP_OPTION_Y, 200)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_FINISH_X, Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_FINISH_Y, 200)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_DIFFICULTY_1_X, Setting.POSITION_LOBBY_CREATE_ROOM_DIFFICULTY_1_Y, 500)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_DIFFICULTY_2_X, Setting.POSITION_LOBBY_CREATE_ROOM_DIFFICULTY_2_Y, 500)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_CHECKBOX_X, Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_CHECKBOX_Y, 500)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_TEXTBOX_X, Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_TEXTBOX_Y, 500)
    if (Setting.SWITCH_LOBBY_CREATE_ROOM_LOCK)
    then
        local password
        if (not Setting.SWITCH_LOBBY_CREATE_ROOM_CUSTOMIZE_PASSWORD) -- 不使用自定义密码
        then
            password = tostring(math.random(10000000, 9999999999999999)) -- 生成 8 ~ 16 位数字密码
        else -- 使用自定义密码
            password = Setting.FIELD_LOBBY_CREATE_ROOM_CUSTOM_PASSWORD
        end
        Keyboard:puts(password)
    end
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_CONFIRM_X, Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_CONFIRM_Y, 500)
    Mouse:click_on(Setting.POSITION_LOBBY_CREATE_ROOM_2_X, Setting.POSITION_LOBBY_CREATE_ROOM_2_Y, 5000) -- 创建房间
end

---点击“开始游戏”按钮，开始游戏。
function Executor:start_game_room()
    Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI) -- 清除所有可能存在的弹窗
    Mouse:click_on(Setting.POSITION_ROOM_START_GAME_X, Setting.POSITION_ROOM_START_GAME_Y, 2000)
end

Executor.last_choose_golden_zombie_reward_time = 0
function Executor:choose_golden_zombie_reward()
    if (not Setting.SWITCH_AUTO_CHOOSE_GOLDEN_ZOMBIE_KILL_REWARDS)
    then
        return
    end
    local t = DateTime:get_local_timestamp()
    if (Executor.last_choose_golden_zombie_reward_time - t > 45)
    then
        return
    end
    Mouse:click_on(
        Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_OPTION_X,
        Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_OPTION_Y,
        300
    )
    Mouse:click_on(
        Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_SELECT_X,
        Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_SELECT_Y,
        300
    )
    Mouse:click_on(
        Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_CONFIRM_X,
        Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_CONFIRM_Y,
        300
    )
    Keyboard:click_several_times(Keyboard.ESCAPE, 4, 10)
    Executor.last_choose_golden_zombie_reward_time = t
end

---选定角色，开始新一轮游戏。
---@return nil
function Executor:choose_character()
    Mouse:click_on_several_times(Setting.POSITION_GAME_ESC_MENU_CANCEL_X, Setting.POSITION_GAME_ESC_MENU_CANCEL_X, 2, 200) -- 点击屏幕上某一处，唤醒窗口（此处点击取消按钮处防止冲突）
    if (Setting.SWITCH_GAME_CHOOSE_TERRORISTS)
    then
        Mouse:click_on_several_times(Setting.POSITION_GAME_CHOOSE_TERRORISTS_TAB_X, Setting.POSITION_GAME_CHOOSE_TERRORISTS_TAB_Y, 2, 500) -- 点击 T 阵营选项卡
    end
    if (Setting.FIELD_GAME_CHARACTER_OPTION and math.type(Setting.FIELD_GAME_CHARACTER_OPTION) == "integer" and Setting.FIELD_GAME_CHARACTER_OPTION >= 0 and Setting.FIELD_GAME_CHARACTER_OPTION <= 9)
    then
        Keyboard:click(tostring(Setting.FIELD_GAME_CHARACTER_OPTION), Delay.NORMAL)
    else
        Console:information("角色选项设置有误。随机选择角色。")
        Keyboard:click(Keyboard.ZERO, Delay.NORMAL)
    end
    Player:reset() -- 重置玩家对象成员变量
end

---上一次尝试确认结算界面的时间戳。
Executor.last_confirm_timestamp = 0
function Executor:try_confirm()
    local current_timestamp = DateTime:get_local_timestamp()
    if (math.abs(current_timestamp - self.last_confirm_timestamp) < 20) -- 未超过 20 秒
    then
        return
    end
    Keyboard:click_several_times(Keyboard.ESCAPE, 10, Delay.MINI) -- 清除所有可能存在的弹窗
    Mouse:click_on(Setting.POSITION_GAME_CONFIRM_RESULTS_X, Setting.POSITION_GAME_CONFIRM_RESULTS_Y)
    self.last_confirm_timestamp = current_timestamp
end

---合成配件。
function Executor:combine_parts()
    if (not Setting.SWITCH_CRAFT_PARTS_BATCH_COMBINE)
    then
        return
    end
    local counter = 20
    Keyboard:press(Keyboard.ENTER, 10)
    repeat
        Mouse:click_on(
            Setting.POSITION_CRAFT_PARTS_FILL_X,
            Setting.POSITION_CRAFT_PARTS_FILL_Y,
            10
        )
        Mouse:click_on(
            Setting.POSITION_CRAFT_PARTS_COMBINE_X,
            Setting.POSITION_CRAFT_PARTS_COMBINE_Y,
            10
        )
        counter = counter - 1
    until (counter == 0)
    Keyboard:release(Keyboard.ENTER, 10)
    Mouse:click_on(
        Setting.POSITION_CRAFT_PARTS_CLEAR_X,
        Setting.POSITION_CRAFT_PARTS_CLEAR_Y,
        20
    )
end

Executor.buy_button_x = 0
Executor.buy_button_y = 0
---购买商店物品。
---@param buy_button_x integer|nil 横坐标。
---@param buy_button_y integer|nil 纵坐标。
function Executor:purchase_item(buy_button_x, buy_button_y)
    if (not Setting.SWITCH_STORE_BATCH_PURCHASE)
    then
        return
    end
    self.buy_button_x = buy_button_x or self.buy_button_x or 0
    self.buy_button_y = buy_button_y or self.buy_button_y or 0
    Mouse:click_on(Executor.buy_button_x, Executor.buy_button_y, 50)
    Mouse:click_on(Setting.POSITION_STORE_PURCHASE_OPTION_X, Setting.POSITION_STORE_PURCHASE_OPTION_Y, 50) -- 弹出界面选项
    Mouse:click_on(Setting.POSITION_STORE_PURCHASE_X, Setting.POSITION_STORE_PURCHASE_Y, 50) -- 弹出界面兑换按钮
    Mouse:click_on(Setting.POSITION_STORE_CONFIRM_PURCHASE_X, Setting.POSITION_STORE_CONFIRM_PURCHASE_Y, 600) -- 兑换后确认
    Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI)
end

---光标定位。
function Executor:locate_cursor()
    if (Keyboard:is_modifier_pressed(Keyboard.CTRL) and Keyboard:is_modifier_pressed(Keyboard.ALT) and not Keyboard:is_modifier_pressed(Keyboard.SHIFT))
    then
        local x, y = Mouse:locate_cursor()
        Console:information("光标位置：(%d, %d)", x, y)
        Runtime:sleep(500)
    end
end

function Executor:clear_popups()
    Keyboard:click_several_times(Keyboard.ESCAPE, 1, 5000)
end
end -- Executor_lua
