local PATH = "C:/Users/Silve/Develop/CSOL-24H/"
dofile(PATH .. "source/Player.lua")
dofile(PATH .. "source/Keyboard.lua")
dofile(PATH .. "source/Mouse.lua")
dofile(PATH .. "source/Weapon.lua")
dofile(PATH .. "source/Utility.lua")
dofile(PATH .. "source/Console.lua")

if (not Round)
then
    Round = {}
    -- 一局时间（非游戏中一局真实时间），原因不再赘述
    Round.ROUND_TIME = 60 * 1000
    -- 加载进度条时间
    Round.LOAD_TIME = 30 * 1000
    -- 玩家列表
    Round.playerList = {}
    Round.playerNumber = 1
    -- 使用的虚拟玩家

    -- 初始化，创建玩家
    -- @param nil
    -- @return nil    
    -- 回合计时器
    Round.timer = -114514

    function Round:initialize()
        Round.timer = -114514
        self.defaultPlayer = Player:new()
        self.player = Player:new({
            chiefWeapon = {
                Weapon:new{
                    name = "幻境！光棱剑",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.MELEE,
                    purchaseKeySeq = {Keyboard.B, Keyboard.G},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "【擎空】突击套装",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.SECONDARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.ONE},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "【天驱】突击套装",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.SECONDARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.TWO},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "冰雪魔咒PB-4",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.SECONDARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.THREE},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "次元裁决",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.SECONDARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.FOUR},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "晖耀审判",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.SECONDARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.ONE, Keyboard.FIVE},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "【黯火】加特林",
                    switchDelay = Delay.SHORT,
                    number = Weapon.PRIMARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.FIVE, Keyboard.SEVEN},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "【黯影】Hecate II",
                    switchDelay = Delay.SHORT,
                    number = Weapon.PRIMARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.FOUR, Keyboard.FOUR},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "【恶龙】霰弹炮",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.PRIMARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.TWO, Keyboard.ONE},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "【异噬】暗雷兽",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.PRIMARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.TWO, Keyboard.TWO},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "暗狱雷魂",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.PRIMARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.EIGHT, Keyboard.THREE},
                    attackButton = Mouse.LEFT
                },
                Weapon:new{
                    name = "逆界星轮",
                    switchDelay = Delay.NORMAL,
                    number = Weapon.PRIMARY,
                    purchaseKeySeq = {Keyboard.B, Keyboard.EIGHT, Keyboard.SIX},
                    attackButton = Mouse.RIGHT
                }
            },
            partWeapon = {},
            -- 辅助武器默认使用天使雷
            -- 主要武器概率分布
            chiefWeaponProbabilityDistribution = {0.1, 0.2, 0.28, 0.36, 0.44, 0.52, 0.60, 0.68, 0.76, 0.84, 0.92, 1.00}
        })
        self.playerList = {
            self.defaultPlayer,
            self.player
        }
        Console:infomation(string.format("Create %d player(s).", #self.playerList))
        self:chooseMainPlayer(true)
    end

    -- 重置一局，选定角色
    -- @param nil
    -- @return nil
    Round.isResetting = false
    function Round:reset()
        Round.isResetting = true
        Console:infomation("Start resetting round.")
        local moment = Runtime:execTime()
        self.playerList[self.playerNumber].useDefaultWeapon = true
        repeat
            self.playerList[self.playerNumber]:purchaseChiefWeapon()
            Weapon:closePreorderMenu()
            -- self.playerList[self.playerNumber]:startMove()
            self.playerList[self.playerNumber]:startAttack()
            self.playerList[self.playerNumber]:turn()
            self.playerList[self.playerNumber]:stopAttack()
            -- self.playerList[self.playerNumber]:stopMove()
        until Runtime:execTime() - Round.LOAD_TIME > moment or Runtime.exit
        Round:chooseClass(false)
        self.playerList[self.playerNumber].useDefaultWeapon = false
        Console:infomation("Finish resetting round.")
        Round.isResetting = false
    end

    -- 按ESC键清除所有UI
    -- @param nil
    -- @return nil
    function Round:clearUI()
        Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.NORMAL)
        Mouse:double_click_on(Setting.ESC_MENU_CANCEL_X, Setting.ESC_MENU_CANCEL_Y) -- 点击ESC菜单的取消按钮
        Mouse:double_click_on(Setting.GAME_SETTLEMENT_CONFIRM_X, Setting.GAME_SETTLEMENT_CONFIRM_Y) -- 结算界面确认
    end

    -- 角色选项
    Round.classOption = Keyboard.THREE
    -- 选定角色
    -- @param nil
    -- @return nil
    function Round:chooseClass(isTerrorist)
        -- if (isTerrorist)
        -- then
        --     for _ = 1, 5
        --     do
        --         Mouse:clickOn(11304, 9596) -- 选择 T 阵营人物
        --     end
        -- end
        for _ = 1, 5
        do
            Keyboard:click(self.classOption, Delay.SHORT)
        end
    end

    function Round:chooseMainPlayer(confirm)
        confirm = confirm or (Keyboard:is_modifier_pressed(Keyboard.LCTRL) and Keyboard:is_modifier_pressed(Keyboard.RCTRL))
        local number = Keyboard:getKeyLockState()
        if (confirm and number > 0 and number <= #self.playerList)
        then
            self.playerNumber = number
            return true
        end
        self.playerNumber = 1
        return false
    end


    -- 开始挂机
    -- @param nil
    -- @return nil
    function Round:start()
        self:initialize() -- 初始化
        while (not Runtime.exit)  -- 没有要求退出
        do
            if (Keyboard:getKeyLockState() == 0) -- 锁存器值为0，进入自陷状态
            then
                Console:infomation("Procedure Round:start() is suspended.")
                local exit = Runtime:waitFor(
                    function()
                        while (not(Runtime.exit or self:chooseMainPlayer(false))) -- 未发出退出或者继续指令则保持挂起
                        do
                            Runtime:sleep(Delay.LONG_LONG)
                        end
                        return Runtime.exit
                    end
                )
                
                if (exit) -- 退出
                then
                    Console:infomation("Procedure Round:start() will exit.")
                    Keyboard:release_all(Delay.SHORT)
                    Mouse:release_all(Delay.SHORT)
                    break
                end
                Console:infomation(string.format("Player Number: %d", self.playerNumber))
                Console:infomation("Procedure Round:start() is resumed.")
            end
            if (Runtime:execTime() - Round.timer > Round.ROUND_TIME)
            then
                Mouse:double_click(Setting.GAMESTART_X, Setting.GAMESTART_Y)
                self:reset()
                self.timer = Runtime:execTime()
            end
            -- 概率购买配件和辅助武器，防止购买次数过多
            local random = Utility:generateRandom()
            if (random <= 0.33)
            then
                self.playerList[self.playerNumber]:purchasePartWeapon()
            elseif (random <= 0.67)
            then
                self.playerList[self.playerNumber]:purchaseAuxiliaryWeapon()
            end
            self.playerList[self.playerNumber]:purchaseChiefWeapon()
            self:clearUI() -- 清空所有界面或游戏结算界面确认
            self.playerList[self.playerNumber]:startMove()
            self.playerList[self.playerNumber]:startAttack()
            self.playerList[self.playerNumber]:turn()
            self.playerList[self.playerNumber]:stopAttack()
            self.playerList[self.playerNumber]:stopMove()
            self.playerList[self.playerNumber]:useAuxiliaryWeapon()
            self.playerList[self.playerNumber]:purchaseAC()
        end
        Console:infomation("Procedure Round:start() has exited.")
    end
    function Round:resultConfirm()
        Keyboard:click_several_times(Keyboard.ESCAPE, 4, Delay.NORMAL) -- 连续按数次 ESC
        Mouse:double_click_on(Setting.ESC_MENU_CANCEL_X, Setting.ESC_MENU_CANCEL_Y) -- 点击ESC菜单的取消按钮
        Mouse:double_click_on(Setting.GAME_SETTLEMENT_CONFIRM_X, Setting.GAME_SETTLEMENT_CONFIRM_Y) -- 结算界面确认
        Runtime:sleep(3000)
    end

    function Round:startGame()
        Mouse:double_click_on(Setting.GAMESTART_X, Setting.GAMESTART_Y)
        Runtime:sleep(Round.LOAD_TIME)
        Round:chooseClass()
    end
    function Round:play(playerNumber)
        local player = self.playerList[playerNumber] or self.playerList[0]
        -- 概率购买配件和辅助武器，防止购买次数过多
        local random = Utility:generateRandom()
        if (random <= 0.33)
        then
            player:purchasePartWeapon()
        elseif (random <= 0.67)
        then
            player:purchaseAuxiliaryWeapon()
        end
        player:purchaseChiefWeapon()
        player:startMove()
        player:startAttack()
        player:turn()
        player:stopAttack()
        player:stopMove()
        player:useAuxiliaryWeapon()
        player:purchaseAC()
    end
end
