if (not Weapon_lua)
then
Weapon_lua = true
Load("Runtime.lua")
Load("Delay.lua")
Load("Console.lua")
Load("Mouse.lua")
Load("Keyboard.lua")

---@class Weapon
Weapon = {}

---主武器。
---@type string
Weapon.PRIMARY = Keyboard.ONE

---副武器。
---@type string
Weapon.SECONDARY = Keyboard.TWO

---近身武器。
---@type string
Weapon.MELEE = Keyboard.THREE

---手雷。
---@type string
Weapon.GRENADE = Keyboard.FOUR

---武器名称。
---@type string
Weapon.name = "未知"

---武器序号。
---@type string
Weapon.number = Weapon.PRIMARY

---武器攻击鼠标按钮，默认为鼠标左键。
---@type integer
Weapon.attack_button = Mouse.LEFT

---武器切换延迟，单位为毫秒，默认为 `Delay.MEDIUM`。
---@type integer
Weapon.switch_delay = Delay.MEDIUM

---购买该武器的键盘按键序列。
---@type table
Weapon.purchase_sequence = {}

---创建一个武器对象
---@param obj table 初始化列表。
---@return table 武器对象
function Weapon:new(obj)
    obj = obj or {}
    self.__index = self
    setmetatable(obj, self)
    Console:infomation(
        "创建武器对象：" .. obj.name
    )
    return obj
end

---根据 `purchase_sequence` 字段中预设的按键序列购买武器。
---@return nil
function Weapon:purchase()
    for _, key in ipairs(self.purchase_sequence)
    do
        Keyboard:click(key, Delay.NORMAL)
    end
    Keyboard:click(Weapon.MELEE, Delay.NORMAL) -- 购买完成后，临时切换到近战武器，防止后续鼠标点击导致使用诸如燃爆等武器。
    -- 清除当前界面上的所有窗口，防止购买资金不足或关闭死亡购买界面。
    Keyboard:click_several_times(Keyboard.ESCAPE, 6, Delay.MINI)
    Mouse:click_on(Setting.ZS_GAME_ESC_MENU_CANCEL_X, Setting.ZS_GAME_ESC_MENU_CANCEL_Y, 20) -- 点击ESC菜单的取消按钮。
    -- f:in_case_insufficient_funds()
    -- self:close_dead_purchase_menu()
end

---切换到指定武器。
---@return nil
function Weapon:switch()
    Keyboard:click(self.number, self.switch_delay)
end

---按下 `Keyboard.G` 键丢弃武器。
---@return nil
function Weapon:abandon()
    self:switch() -- 切换到指定武器。
    Keyboard:click(Keyboard.G, Delay.NORMAL)
end

---确认武器购买资金不足提示框（预设按钮在 Setting.lua 中）。
---@return nil
function Weapon:in_case_insufficient_funds()
    Mouse:click_on(Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_X, Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_Y)
    Keyboard:click(Keyboard.ZERO, Delay.SHORT)
end

---关闭死亡状态下的预购买菜单（点击“重复购买”按钮，不点击“取消购买”以避免与大厅界面按钮冲突）。
---@return nil
function Weapon:close_dead_purchase_menu()
    Mouse:click_on(Setting.GAME_DEAD_PURCHASE_MENU_REBUY_X, Setting.GAME_DEAD_PURCHASE_MENU_REBUY_Y)
end

---使用特殊武器的函数，在创建特殊武器对象时重写此函数。
function Weapon:use()
end

---开始使用该武器攻击
---@return nil
function Weapon:start_attack()
    Mouse:press(self.attack_button)
end

---停止使用该武器攻击
---@return nil
function Weapon:stop_attack()
    Mouse:release(self.attack_button)
end

end -- Weapon_lua
