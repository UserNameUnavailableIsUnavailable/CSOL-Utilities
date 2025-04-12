if not Automation_lua then
    Automation_lua = true
    Include("Delay.lua")
    Include("Console.lua")
    Include("JSON.lua")
    Include("Context.lua")
    Include("Error.lua")
    Include("Runtime.lua")
    Include("Keyboard.lua")
    Include("Mouse.lua")
    Include("DateTime.lua")
    Include("Command.lua")
    Include("Utility.lua")
    Include("Weapon.lua")
    Include("Player.lua")
    Include("Setting.lua")
    Include("WeaponList.lua")
    Include("Version.lua")
    Version:set("Automation", "1.5.2")
    Automation = {}

    ---手动接管标识。
    Runtime.manual_flag = false
    ---注册暂停事件处理函数，处理用户手动接管事件。
    Runtime:register_interrupt(Interrupt:new({
        name = "手动接管功能",
        handler = function()
            if
                Keyboard:is_modifier_pressed(Keyboard.LEFT_CTRL) and Keyboard:is_modifier_pressed(Keyboard.RIGHT_CTRL)
            then
                Keyboard:reset()
                Mouse:reset()
                if not Runtime.manual_flag then
                    Console:information("开始手动接管，冻结键鼠操作。")
                    Keyboard:freeze()
                    Mouse:freeze()
                end
                Runtime.manual_flag = true
            elseif
                Keyboard:is_modifier_pressed(Keyboard.LEFT_ALT) and Keyboard:is_modifier_pressed(Keyboard.RIGHT_ALT)
            then
                if Runtime.manual_flag then
                    Console:information("中止手动接管，恢复键鼠操作。")
                    Keyboard:unfreeze()
                    Mouse:unfreeze()
                end
                Runtime.manual_flag = false
            end
        end,
        maskable = false, -- 此中断不可屏蔽
    }))

    Automation.last_reset_or_respawn_time = 0
    ---注册回合重置检查函数，游戏开始后每隔 8 秒进行回合重置。
    Runtime:register_interrupt(Interrupt:new({
        name = "复活、回合重置功能",
        handler = function()
            -- 当前未在挂机
            local cmd = Command:claim()
            if cmd ~= Command.CMD_DEFAULT_IDLE and cmd ~= Command.CMD_EXTENDED_IDLE then
                return
            end
            local time = Runtime:get_running_time()
            if Setting.SWITCH_GAME_RESET_ROUND_ON_FAILURE and time - Automation.last_reset_or_respawn_time > 8000 then
                Player:reset_round_or_respawn()
                Automation.last_reset_or_respawn_time = time
            end
        end,
    }))

    -- 初始化
    Error:register_error_handler("COMMAND_CHANGED", function() end)
    DateTime:set_time_zone(Setting.FIELD_TIME_ZONE) -- 时区
    Weapon:set_reload_key(Setting.KEYSTROKES_GAME_RESET_ROUND_KEY[1]) -- 换弹按键
    Player:set_respawn_key(Setting.KEYSTROKES_GAME_RESET_ROUND_KEY[1]) -- 复活按键

    -- 为默认挂机模式和扩展挂机模式创建两个玩家对象。

    ---默认模式挂机玩家对象。
    Automation.default_player = Player:new({
        armor = Armor,
        part_weapons = DefaultPartWeapons,
        conventional_weapons = DefaultConventionalWeapons,
        special_weapons = DefaultSpecialWeapons,
    })

    ---扩展模式挂机玩家对象。
    Automation.extended_player = Player:new({
        armor = Armor,
        part_weapons = ExtendedPartWeapons,
        conventional_weapons = ExtendedConventionalWeapons,
        special_weapons = ExtendedSpecialWeapons,
    })

    Runtime.last_command_update_timepoint = 0
    Runtime:register_interrupt(Interrupt:new({
        name = "命令即时响应功能",
        handler = function()
            if Runtime:get_running_time() - Runtime.last_command_update_timepoint < 100 then
                return
            end
            Command:update() -- 更新命令
            Runtime.last_command_update_timepoint = Runtime:get_running_time()
            -- 命令类型发生变化，需要立即停止当前执行
            if (Command:get_status() & Command.NAME_CHANGED) == Command.NAME_CHANGED then
                Mouse:reset()
                Keyboard:reset()
                Automation.default_player:reset()
                Automation.extended_player:reset()
                local e = {
                    name = "COMMAND_CHANGED",
                    message = "命令变更",
                    parameters = {},
                }
                Error:throw(e) -- 主动触发运行时错误
            end
        end,
    }))

    ---创建游戏房间。
    function Automation:create_game_room()
        if not Setting.SWITCH_CREATE_ROOM_ON_EXCEPTION then
            return
        end
        Keyboard:click_several_times(Keyboard.ESCAPE, 10, 100, Delay.SHORT) -- 按 10 次 `Keyboard.ESCAPE`，关闭所有弹窗
        Mouse:click_several_times_on(Mouse.LEFT, Setting.POSITION_LOBBY_BACK_X, Setting.POSITION_LOBBY_BACK_Y, 5, 100) -- 按 5 次返回，到大厅进入游戏界面
        Keyboard:click_several_times(Keyboard.ESCAPE, 10, 100, Delay.SHORT) -- 按 10 次 `Keyboard.ESCAPE`，关闭所有弹窗
        Mouse:click_on(Mouse.LEFT, Setting.POSITION_LOBBY_LIST_ROOMS_X, Setting.POSITION_LOBBY_LIST_ROOMS_Y, 500)
        Mouse:click_on(Mouse.LEFT, Setting.POSITION_LOBBY_CREATE_ROOM_1_X, Setting.POSITION_LOBBY_CREATE_ROOM_1_Y, 500)
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_GAME_MODE_1_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_GAME_MODE_1_Y,
            500
        )
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_GAME_MODE_2_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_GAME_MODE_2_Y,
            500
        )
        Mouse:click_several_times_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_LEFT_SCROLL_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_LEFT_SCROLL_Y,
            Setting.FIELD_LOBBY_CREATR_ROOM_MAP_SCROLL_LEFT_COUNT --[[v1.5.1 正式版引入]]
                or 32,
            200
        ) -- 向左翻页指定次数
        -- Mouse:click_several_times_on(Mouse.LEFT, Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_RIGHT_SCROLL_X, Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_RIGHT_SCROLL_Y, Setting.FIELD_LOBBY_CREATR_ROOM_MAP_RIGHT_SCROLL_COUNT, 150
        -- ) -- 向右移动指定次数
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_MAP_OPTION_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_MAP_OPTION_Y,
            750
        ) -- 点击一下，激活子窗口
        Mouse:roll(-(Setting.FIELD_LOBBY_CREATE_ROOM_MAP_SCROLL_DOWN_COUNT --[[v1.5.1 正式版引入]] or 0), 750) -- 向下滚动指定次数
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_MAP_OPTION_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_MAP_OPTION_Y,
            200
        ) -- 选择地图
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_FINISH_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_MAP_CHOOSE_FINISH_Y,
            200
        )
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_DIFFICULTY_1_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_DIFFICULTY_1_Y,
            500
        )
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_DIFFICULTY_2_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_DIFFICULTY_2_Y,
            500
        )
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_CHECKBOX_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_CHECKBOX_Y,
            500
        )
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_TEXTBOX_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_TEXTBOX_Y,
            500
        )
        if Setting.SWITCH_LOBBY_CREATE_ROOM_LOCK then
            local password
            if
                not Setting.SWITCH_LOBBY_CREATE_ROOM_CUSTOMIZE_PASSWORD -- 不使用自定义密码
            then
                password = tostring(math.random(10000000, 9999999999999999)) -- 生成 8 ~ 16 位数字密码
            else -- 使用自定义密码
                password = Setting.FIELD_LOBBY_CREATE_ROOM_CUSTOM_PASSWORD
            end
            Keyboard:puts(password)
        end
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_CONFIRM_X,
            Setting.POSITION_LOBBY_CREATE_ROOM_PASSWORD_CONFIRM_Y,
            500
        )
        Mouse:click_on(Mouse.LEFT, Setting.POSITION_LOBBY_CREATE_ROOM_2_X, Setting.POSITION_LOBBY_CREATE_ROOM_2_Y, 8000) -- 创建房间
    end

    ---点击“开始游戏”按钮，开始游戏。
    function Automation:start_game_room()
        Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI, Delay.SHORT) -- 清除所有可能存在的弹窗
        Mouse:click_on(Mouse.LEFT, Setting.POSITION_ROOM_START_GAME_X, Setting.POSITION_ROOM_START_GAME_Y, 2000)
    end

    -- Automation.last_choose_golden_zombie_reward_time = 0
    function Automation:choose_golden_zombie_reward()
        if
            not Setting.SWITCH_AUTO_CHOOSE_GOLDEN_ZOMBIE_KILL_REWARDS -- 防卡黄金僵尸功能开关
        then
            return
        end
        -- local t = DateTime:get_local_timestamp()
        -- if (Automation.last_choose_golden_zombie_reward_time - t > 45)
        -- then
        --     return
        -- end
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_OPTION_X,
            Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_OPTION_Y,
            300
        )
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_SELECT_X,
            Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_SELECT_Y,
            300
        )
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_CONFIRM_X,
            Setting.POSITION_GOLDEN_ZOMBIE_KILL_REWARDS_CONFIRM_Y,
            300
        )
        -- Automation.last_choose_golden_zombie_reward_time = t
    end

    ---选定角色，开始新一轮游戏。
    ---@return nil
    function Automation:choose_character()
        Mouse:click_several_times_on(
            Mouse.LEFT,
            Setting.POSITION_GAME_ESC_MENU_CANCEL_X,
            Setting.POSITION_GAME_ESC_MENU_CANCEL_X,
            2,
            200
        ) -- 点击屏幕上某一处，唤醒窗口（此处点击取消按钮处防止冲突）
        if Setting.SWITCH_GAME_CHOOSE_TERRORISTS then
            Mouse:click_several_times_on(
                Mouse.LEFT,
                Setting.POSITION_GAME_CHOOSE_TERRORISTS_TAB_X,
                Setting.POSITION_GAME_CHOOSE_TERRORISTS_TAB_Y,
                2,
                500
            ) -- 点击 T 阵营选项卡
        end
        if
            Setting.FIELD_GAME_CHARACTER_OPTION
            and math.type(Setting.FIELD_GAME_CHARACTER_OPTION) == "integer"
            and Setting.FIELD_GAME_CHARACTER_OPTION >= 0
            and Setting.FIELD_GAME_CHARACTER_OPTION <= 9
        then
            Keyboard:click(tostring(Setting.FIELD_GAME_CHARACTER_OPTION), Delay.NORMAL)
        else
            Console:information("角色选项设置有误。随机选择角色。")
            Keyboard:click(Keyboard.ZERO, Delay.NORMAL)
        end
        Player:reset() -- 重置玩家对象成员变量
    end

    ---上一次尝试确认结算界面的时间戳。
    -- Automation.last_confirm_timestamp = 0
    function Automation:try_confirm()
        -- local current_timestamp = DateTime:get_local_timestamp()
        -- if (math.abs(current_timestamp - self.last_confirm_timestamp) < 20) -- 未超过 20 秒
        -- then
        --     return
        -- end
        local cursor_locked = true
        if not Mouse:is_cursor_position_locked() then
            Keyboard:click_several_times(Keyboard.ESCAPE, 3, Delay.MINI, Delay.MINI, true)
            cursor_locked = Mouse:is_cursor_position_locked()
        end
        if cursor_locked then
            return
        end
        Keyboard:click_several_times(Keyboard.ESCAPE, 2, Delay.MINI, Delay.MINI) -- 清除所有可能存在的弹窗
        Automation:choose_golden_zombie_reward() -- 选择黄金僵尸奖励
        Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI, Delay.MINI) -- 清除所有可能存在的弹窗
        Mouse:click_on(Mouse.LEFT, Setting.POSITION_GAME_CONFIRM_RESULTS_X, Setting.POSITION_GAME_CONFIRM_RESULTS_Y) -- 点击确认完成结算
        -- self.last_confirm_timestamp = current_timestamp
    end

    ---合成配件。
    function Automation:combine_parts()
        if not Setting.SWITCH_CRAFT_PARTS_BATCH_COMBINE then
            return
        end
        local counter = 20
        while
            Keyboard:is_modifier_pressed(Keyboard.ALT)
            or Keyboard:is_modifier_pressed(Keyboard.SHIFT)
            or Keyboard:is_modifier_pressed(Keyboard.CTRL)
        do
            Runtime:sleep(10)
        end
        Keyboard:press(Keyboard.ENTER, 10)
        repeat
            Mouse:click_on(Mouse.LEFT, Setting.POSITION_CRAFT_PARTS_FILL_X, Setting.POSITION_CRAFT_PARTS_FILL_Y, 10)
            Mouse:click_on(
                Mouse.LEFT,
                Setting.POSITION_CRAFT_PARTS_COMBINE_X,
                Setting.POSITION_CRAFT_PARTS_COMBINE_Y,
                10
            )
            counter = counter - 1
        until counter == 0
        Mouse:click_on(Mouse.LEFT, Setting.POSITION_CRAFT_PARTS_CLEAR_X, Setting.POSITION_CRAFT_PARTS_CLEAR_Y, 20, true)
        Keyboard:release(Keyboard.ENTER, 10)
    end

    Automation.buy_button_x = 0
    Automation.buy_button_y = 0
    ---购买商店物品。
    ---@param buy_button_x integer|nil 横坐标。
    ---@param buy_button_y integer|nil 纵坐标。
    function Automation:purchase_item(buy_button_x, buy_button_y)
        if not Setting.SWITCH_STORE_BATCH_PURCHASE then
            return
        end
        self.buy_button_x = buy_button_x or self.buy_button_x or 0
        self.buy_button_y = buy_button_y or self.buy_button_y or 0
        Mouse:click_on(Mouse.LEFT, Automation.buy_button_x, Automation.buy_button_y, 30)
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_STORE_PURCHASE_OPTION_X,
            Setting.POSITION_STORE_PURCHASE_OPTION_Y,
            30
        ) -- 弹出界面选项
        Mouse:click_on(Mouse.LEFT, Setting.POSITION_STORE_PURCHASE_X, Setting.POSITION_STORE_PURCHASE_Y, 30) -- 弹出界面兑换按钮
        Mouse:click_on(
            Mouse.LEFT,
            Setting.POSITION_STORE_CONFIRM_PURCHASE_X,
            Setting.POSITION_STORE_CONFIRM_PURCHASE_Y,
            600
        ) -- 兑换后确认
        Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.MINI, Delay.MINI)
    end

    ---光标定位。
    function Automation:locate_cursor()
        if
            Keyboard:is_modifier_pressed(Keyboard.CTRL)
            and Keyboard:is_modifier_pressed(Keyboard.ALT)
            and not Keyboard:is_modifier_pressed(Keyboard.SHIFT)
        then
            local x, y = Mouse:locate()
            Console:information("位置坐标：(%d, %d)", x, y)
            Runtime:sleep(500)
        end
    end

    function Automation:clear_popups()
        Keyboard:click(Keyboard.ESCAPE, 5000)
    end
end -- Automation_lua
