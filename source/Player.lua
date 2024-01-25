local PATH = "C:/Users/Silve/Develop/Projects/CSOL24H/"
dofile(PATH .. "source/Keyboard.lua")
dofile(PATH .. "source/Mouse.lua")
dofile(PATH .. "source/Weapon.lua")
dofile(PATH .. "source/Utility.lua")

if (not Player)
then
    Player = {}
    -- 跑动方向，0表示向前，1表示向后
    Player.runDirection = 1
    -- 扫射方向，0表示向右，1表示向左
    Player.strafeDirection = 1
    -- 开始按照随机的方向Player.runDirection和Player.strafeDirection移动
    -- @param nil
    -- @return nil
    function Player:startMove()
        self.runDirection = Utility:generateRandom(0, 1)
        self.strafeDirection = Utility:generateRandom(0, 1)
        if (self.runDirection == 0)
        then
            Keyboard:press(Keyboard.W, Delay.NORMAL)
        else
            Keyboard:press(Keyboard.S, Delay.NORMAL)
        end
        if (self.strafeDirection == 0)
        then
            Keyboard:press(Keyboard.D, Delay.NORMAL)
        else
            Keyboard:press(Keyboard.A, Delay.NORMAL)
        end
    end

    -- 结束移动
    -- @param nil
    -- @return nil
    function Player:stopMove()
        if (self.runDirection == 0)
        then
            Keyboard:release(Keyboard.W, Delay.NORMAL)
        else
            Keyboard:release(Keyboard.S, Delay.NORMAL)
        end
        if (self.strafeDirection == 0)
        then
            Keyboard:release(Keyboard.D, Delay.NORMAL)
        else
            Keyboard:release(Keyboard.A, Delay.NORMAL)
        end
    end

    -- Player对象
    -- 一次游戏中可以创建多个Player对象，每个对象可以使用不同的武器序列，按照不同的方式移动，一切根据自己需要来改写Player中的方法，或者提供新方法

    -- 一些概念说明如下
    -- 主要武器chief weapon：用来攻击的武器，如魔法刀
    -- 配件武器part weapon：配件武器，不用于攻击
    -- 辅助武器auxiliary weapon：可与主要武器并行攻击，如天使雷
    -- 说明
    -- 对于主要武器（无特殊攻击，如神鬼左R），只需要按照如下所示的方式创建三张武器列表
    -- 对于配件武器，按照如下方式创建武器列表即可
    -- 对于辅助武器，需要自行实现使用辅助武器的函数Player：useAuxiliaryWeapon
    -- 推荐做法
    -- 对于特殊攻击方式的武器，在创建武器对象时，除继承Weapon类的方法外，应当重写其中与攻击有关的方法，
    -- 或者可以单独提供攻击的方法（见Player：auxiliaryWeapon创建示例，其中提供了use函数，use函数在Player:useAuxiliaryWeapon被调用）
    -- 并对应地自行实现Player：useChiefWeapon或Player.useAuxliaryWeapon

    -- 使用的默认主要武器序列
    -- 强烈建议主要武器序列中第一件武器为带恢复配件武器（如魔法刀），且其购买按键序列不包含数字，原因如下：

    -- 本程序照如下流程执行：
    --   ------------------------------------------------------------------------------
    --   |                                                                             |
    --   \/                                                                            |
    -- [ 开 始 游 戏 ]->[ 重 置 状 态 (每隔一段时间发生一次) ]->[ 正常购买、攻击流程 ]------
    --                    等待加载
    --                    购买默认武器
    --                    攻击
    --                    选择人物
    --                
    -- 当程序开始运行并加载进入游戏时，会在一段时间内（重置状态）重复购买主要武器序列中的第一件武器，且在这段时间内仅用该武器攻击，然后执行选择人物的操作。
    -- 每局游戏的时间较长，且时间根据进入游戏的时刻不同变化很大。因此为了尽快进入游戏（尤其是房主，全部准备完毕后等待时间超过60秒强制被踢；对于一般玩家，长时间不进入游戏也会被踢），
    -- 将重置状态发生的条件设定为每隔一段固定的较短时间（如一分钟）触发一次。这样，每隔一分钟，就会执行点击“开始游戏”按钮的操作，
    -- 点击“开始游戏”后即认为游戏正在加载进度条（保守起见预设为20秒，实际一般15秒左右），但是实际情况很可能是是一局游戏正在进行，因此这段加载时间不能什么都不做，必须要使用某件武器
    -- 进行攻击。故此，在这20秒的时间间隔内，仅购买并使用默认的主要武器（chiefWeapon武器列表的第一项武器）攻击。
    -- 当20秒时间间隔结束，执行选择人物操作。如果之前默认的主要武器购买按键序列中包含数字，就会导致选错人物。
    -- 另外，考虑到真正游戏开局血量、资金等问题，也强烈建议将第一件武器设置成恢复配件的近战武器。
    Player.chiefWeapon = {
        Weapon:new({
            name = "幻境！光棱剑",
            switchDelay = Delay.NORMAL,
            number = Weapon.MELEE,
            purchaseKeySeq = {Keyboard.B, Keyboard.G},
            attackButton = Mouse.LEFT})
    }

    -- 使用的默认配件武器序列
    Player.partWeapon = {
        Weapon:new({
            name = "星战前线·加特林",
            number = Weapon.DONT_USE,
            purchaseKeySeq = {Keyboard.B, Keyboard.J, Keyboard.TWO, Keyboard.FOUR}}),
        Weapon:new({
            name = "FNP-45战损版",
            number = Weapon.DONT_USE,
            purchaseKeySeq = {Keyboard.B, Keyboard.J, Keyboard.ONE, Keyboard.TWO}})
    }

    -- 使用的默认辅助武器序列
    Player.auxiliaryWeapon = {
        Weapon:new({
            name = "圣翼皓印",
            switchDelay = Delay.LONG_LONG,
            number = Weapon.GRENADE ,
            purchaseKeySeq = {Keyboard.B, Keyboard.EIGHT, Keyboard.NINE},
            discharging = false,
            turnOnMoment = -114514,
            turnOffMoment = -114514,
            DISCHARGE_TIME = 24 * 1000,
            RECHARGE_TIME = 9 * 1000,
            use = function (self)
                if (not self.discharging and Runtime:execTime() - self.turnOffMoment > self.RECHARGE_TIME)
                then
                    self.discharging = true
                    self.turnOnMoment = Runtime:execTime()
                    self:switch()
                    Mouse:click(Mouse.LEFT, Delay.NORMAL)
                elseif (self.discharging and Runtime:execTime() - self.turnOnMoment > self.DISCHARGE_TIME)
                then
                    self.discharging = false
                    self.turnOffMoment = Runtime:execTime()
                    self:switch()
                    Mouse:moveRelative(0, 4000, Delay.NORMAL)
                    Keyboard:click(Keyboard.R, self.switchDelay)
                    Mouse:moveRelative(0, -4000, Delay.NORMAL)
                end
            end
        })
    }

    -- 护甲
    Player.AC = 
        Weapon:new({
            name = "避弹衣+头盔",
            purchaseKeySeq = {Keyboard.B, Keyboard.EIGHT, Keyboard.TWO},
            number = 0
        })

    -- 进入正常游戏状态（非重置状态）后，按照下面概率分布，随机地购买主要武器；对于配件和辅助武器，采用循环购买方式，如欲使用随机购买可仿照主要武器随机购买的例程自行实现。
    -- 概率分布相关知识请参见概率论，不予赘述。
    -- 主要武器的购买概率分布，项数要与主要武器列表项数匹配，由于主要武器列表只有一件武器，仅有一项1（后面还会展示如何创建多项，并创建多个Player实例）
    Player.chiefWeaponProbabilityDistribution = { 1 }

    -- 每次购买完成后，会将下面要使用的武器设置成刚刚买好的武器
    Player.chiefWeaponToUse = Player.chiefWeapon[1]
    Player.auxiliaryWeaponToUse = Player.auxiliaryWeapon[1]

    -- 是否仅使用默认武器，在重置状态下使用该字段
    Player.useDefaultWeapon = false

    -- nextPartWeaponToBuy是一个计数器（在Player:purchasePartWeapon中进行初始化），每次调用该计数器返回一个整数值
    -- 该整数值代表了Player.partWeapon表的下标
    Player.nextPartWeaponToBuy = nil

    -- 按照次序，根据计数器返回值循环地购买配件武器
    -- @param nil
    -- @return nil
    function Player:purchasePartWeapon()
        if (not self.partWeapon or #self.partWeapon == 0)
        then
            return
        end
        if (not self.nextPartWeaponToBuy) -- 计数器尚未被初始化，则创建一个赋给nextPartWeaponToBuy
        then
            self.nextPartWeaponToBuy = Utility:counter(1, #self.partWeapon) 
        end
        self.partWeapon[self:nextPartWeaponToBuy()]:purchase() -- 调用计数器函数获取下一件要购买和使用的武器
    end

    -- 购买辅助武器（准备使用该武器），同上
    Player.nextAuxiliaryWeaponToUse = nil
    -- 按照次序，根据计数器返回值循环地购买辅助武器，并在下次使用该武器
    -- @param nil
    -- @return nil
    function Player:purchaseAuxiliaryWeapon()
        local weapon
        if (not self.auxiliaryWeapon or #self.auxiliaryWeapon == 0)
        then
            return
        end
        if (not self.nextAuxiliaryWeaponToUse)
        then
            self.nextAuxiliaryWeaponToUse = Utility:counter(1, #self.auxiliaryWeapon)
        end
        weapon = self.auxiliaryWeapon[self.nextAuxiliaryWeaponToUse()]
        weapon:purchase()
        self.auxiliaryWeaponToUse = weapon
        self.chiefWeaponToUse:switch() -- 购买之后换回主要武器
    end

    -- 为了防止连续使用同一把主/副武器（子弹用完导致空转），增设以下字段用于区分
    Player.lastUsedPrimaryWeapon = "" -- 最近一次使用的主武器
    Player.lastUsedSecondaryWeapon = "" -- 最近一次使用的副武器
    -- 随机购买主要武器，并在下次使用该武器
    -- @param nil
    -- @return nil
    function Player:purchaseChiefWeapon()
        if (self.useDefaultWeapon or #self.chiefWeapon == 1) -- 默认武器位使能，或武器列表只有一件武器
        then
            self.chiefWeaponToUse = self.chiefWeapon[1]
        else
            local weapon
            local random
            repeat
                random = Utility:generateRandom()
                for i = 1, #self.chiefWeaponProbabilityDistribution
                do
                    if (random < self.chiefWeaponProbabilityDistribution[i]) -- 根据概率分布确定被随机到的武器
                    then
                        weapon = self.chiefWeapon[i]
                        break
                    end
                end
            until weapon.name ~= self.lastUsedPrimaryWeapon and weapon.name ~= self.lastUsedSecondaryWeapon
            if (weapon.number == Weapon.PRIMARY)
            then
                self.lastUsedPrimaryWeapon = weapon.name
            elseif (weapon.number == Weapon.SECONDARY)
            then
                self.lastUsedSecondaryWeapon = weapon.name
            end
            weapon:purchase()
            self.chiefWeaponToUse = weapon
        end
        self.chiefWeaponToUse:switch()
    end

    -- 使用辅助武器
    function Player:useAuxiliaryWeapon()
        self.auxiliaryWeaponToUse:use()
    end

    -- 购买护甲
    function Player:purchaseAC()
        self.AC:purchase()
    end

    -- 随机使用血包
    -- @param [(table)probabilityDistribution] 概率分布表
    -- @return nil
    -- @remark 依照概率分布随机使用血包（5和6），故分布表应有两项
    function Player:randomlyUseMedicalKits(probabilityDistribution)
        probabilityDistribution = probabilityDistribution or {0.1, 0.15}
        local random = Utility:generateRandom()
        if (random < probabilityDistribution[1])
        then
            Keyboard:press(Keyboard.FIVE, Delay.SHORT)
        elseif (random < probabilityDistribution[2])
        then
            Keyboard:press(Keyboard.SIX, Delay.SHORT)
        end
    end

    -- 创建Player实例
    -- @param [obj] 初始化列表
    -- @return nil
    -- @remark 如果选择直接创建Player实例，则默认使用类中预定义好的若干内容
    function Player:new(obj)
        self.__index = self
        obj = obj or {}
        setmetatable(obj, self)
        return obj
    end

    -- 一次转圈的时长
    Player.turnDuration = 5.5 * 1000

    -- 以随机方向、随机速度转圈
    -- @param nil
    -- @return nil
    function Player:turn()
        local rotateStopMoment = Runtime:execTime() + self.turnDuration
        local sensitivity = (2 - Utility:generateRandom()) / 2 -- 灵敏度∈(0.5, 1]
        local direction
        if (Utility:generateRandom(0, 1) == 0)
        then
            direction = 1
        else
            direction = -1
        end
        repeat
            Mouse:moveRelative(100 * direction * sensitivity, 0, Delay.MINI)
        until Runtime:execTime() > rotateStopMoment or Runtime.exit
        Keyboard:click(Keyboard.R, Delay.SHORT)
    end

    function Player:startAttack()
        self.chiefWeaponToUse:startAttack()
    end

    function Player:stopAttack()
        self.chiefWeaponToUse:stopAttack()
    end
end
