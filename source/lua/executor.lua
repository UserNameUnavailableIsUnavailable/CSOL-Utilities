Load("const.lua")
Load("Runtime.lua")
Load("Mouse.lua")
Load("Keyboard.lua")
Load("Setting.lua")

function StartGameRoom()
    Keyboard:click_several_times(Keyboard.ESCAPE, 2)
    Mouse:click_on(Setting.GAMESTART_X, Setting.GAMESTART_Y)
end

---选定角色。
---@param is_terrorist boolean | nil 是否需要选择恐怖分子阵营。
---@param choice integer | string 角色选项，可取 `"0" ~ "9"`。
---@return nil
function ChooseClass(is_terrorist, choice)
    if (is_terrorist)
    then
        Mouse:click_on(Setting.CHOOSE_T_CLASS_X, Setting.CHOOSE_T_CLASS_Y)
    end
end

function TryConfirm()
    Keyboard:click_several_times(Keyboard.ESCAPE, 2)
    Mouse:click_on(Setting.GAME_SETTLEMENT_CONFIRM_X, Setting.GAME_SETTLEMENT_CONFIRM_Y)
end