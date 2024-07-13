if (not Executor_lua)
then
Executor_lua = true
Executor = {}

function Executor:start_game_room()
    -- Keyboard:click_several_times(Keyboard.ESCAPE, 2, Delay.NORMAL)
    -- Mouse:click_on(Setting.GAMESTART_X, Setting.GAMESTART_Y)
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
end

function Executor:try_confirm()
    Keyboard:click_several_times(Keyboard.ESCAPE, 2)
    Mouse:click_on(Setting.GAME_SETTLEMENT_CONFIRM_X, Setting.GAME_SETTLEMENT_CONFIRM_Y)
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

end -- Executor_lua