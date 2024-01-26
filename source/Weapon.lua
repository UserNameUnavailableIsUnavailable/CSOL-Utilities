local PATH = "C:/Users/Silve/Develop/Projects/CSOL24H/"
dofile(PATH .. "source/Console.lua")
dofile(PATH .. "source/Runtime.lua")
if (not Weapon)
then
    Weapon = {
        PRIMARY = Keyboard.ONE,-- 主武器
        SECONDARY = Keyboard.TWO, -- 副武器
        MELEE = Keyboard.THREE, -- 近身武器
        GRENADE = Keyboard.FOUR, -- 手雷
        name = '', -- 武器名
        DONT_USE = 0, -- 不使用该武器
        number = 0, -- 第几号武器栏
        purchaseKeySeq = {}, -- 武器购买按键序列
        switchDelay = Delay.MEDIUM, -- 武器切枪延迟
        attackButton = Mouse.LEFT, -- 攻击按键
    }

     -- 创建一个武器对象
    -- @param (table)[obj] 初始化列表
    -- @return table 武器对象
    function Weapon:new(obj)
        obj = obj or {}
        self.__index = self
        setmetatable(obj, self)
        Console:infomation(
            "新增武器：" .. obj.name
        )
        return obj
    end

    -- 根据purchaseKeySeq字段中预设的按键购买武器
    -- @param nil
    -- return nil
    function Weapon:purchase()
        for _, key in ipairs(self.purchaseKeySeq)
        do
            Keyboard:click(key, Delay.MEDIUM)
        end
    end
    function Weapon:abandon()
        self:switch()
        Keyboard:click(Keyboard.G, Delay.NORMAL)
    end
    -- 切换到指定武器
    -- @param weapon 武器对象
    -- @return nil
    function Weapon:switch()
        Keyboard:click(self.number, self.switchDelay)
    end
    -- 确认武器购买资金不足提示框（预设按钮在Setting中）
    -- @param nil
    -- @return nil
    function Weapon:inCasePurchaseFailsForInsuffFunds()
        Mouse:clickOn(Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_X, Setting.GAME_INSUFFIENT_FUNDS_CONFIRM_Y)
        Keyboard:click(Keyboard.ZERO, Delay.SHORT)
    end

    -- 关闭死亡状态下的预购买菜单（点击“重复购买”按钮，不点击“取消购买”以避免与大厅界面按钮冲突）
    -- @param nil
    -- @return nil
    function Weapon:closePreorderMenu()
        Mouse:clickOn(Setting.GAME_PREORDER_MENU_REBUY_X, Setting.GAME_PREORDER_MENU_REBUY_Y)
    end

    -- 开始使用该武器攻击
    -- @param nil
    -- @return nil
    function Weapon:startAttack()
        Mouse:press(self.attackButton)
    end

    -- 停止使用该武器攻击
    -- @param nil
    -- @return nil
    function Weapon:stopAttack()
        Mouse:release(self.attackButton)
    end
end
