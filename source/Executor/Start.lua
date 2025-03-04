if (not Start_lua)
then
    Start_lua = true
    Include("Error.lua")

    Error:register_error_handler(
        "COMMAND_CHANGED",
        function ()
        end
    )

    Runtime.last_command_update_timepoint = 0
    Runtime:register_interrupt_handler(
        function ()
            if (Runtime:get_running_time() - Runtime.last_command_update_timepoint < 100)
            then
                return
            end
            Command:update() -- 更新命令
            Runtime.last_command_update_timepoint = Runtime:get_running_time()
            -- 命令类型发生变化，需要立即停止当前执行
            if ((Command:get_status() & Command.NAME_CHANGED) == Command.NAME_CHANGED)
            then
                Mouse:reset()
                Keyboard:reset()
                Player:reset()
                local e = {
                    name = "COMMAND_CHANGED",
                    message = "命令变更",
                    parameters = {}
                }
                Error:throw(e) -- 主动触发运行时错误
            end
        end
    )

    local function interpret()
        local cmd = Command:claim() -- 领取任务
        if (cmd == Command.CMD_START_GAME_ROOM) -- 开始游戏
        then
            Executor:start_game_room()
        elseif (cmd == Command.CMD_CHOOSE_CHARACTER) -- 选定角色
        then
            Executor:choose_character()
        elseif (cmd == Command.CMD_DEFAULT_IDLE) -- 24H 挂机模式（常规）
        then
            Executor:try_confirm()
            Executor:choose_golden_zombie_reward()
            Player:use_special_weapon(DefaultSpecialWeapons)
            Player:buy_part_weapon(DefaultPartWeapons)
            Player:play(DefaultConventionalWeapons)
        elseif (cmd == Command.CMD_EXTENDED_IDLE) -- 24H 挂机模式（扩展）
        then
            Executor:try_confirm()
            Executor:choose_golden_zombie_reward()
            Player:use_special_weapon(ExtendedSpecialWeapons)
            Player:buy_part_weapon(ExtendedPartWeapons)
            Player:play(ExtendedConventionalWeapons)
        elseif (cmd == Command.CMD_CREATE_GAME_ROOM) -- 创建房间功能
        then
            Executor:create_game_room()
        elseif (cmd == Command.CMD_BATCH_COMBINE_PARTS) -- 合成配件功能
        then
            Executor:combine_parts()
        elseif (cmd == Command.CMD_BATCH_PURCHASE_ITEM) -- 购买物品功能
        then
            -- 对于新发出的命令，需要更新鼠标光标位置
            if ((Command:get_status() & Command.NAME_CHANGED) ~= Command.UNCHANGED) -- 新旧命令类型不同，即旧命令不是购买物品
            then
                Executor:purchase_item(Mouse:locate())
            else
                Executor:purchase_item()
            end
        elseif (cmd == Command.CMD_LOCATE_CURSOR) -- 光标定位功能
        then
            Executor:locate_cursor()
        elseif (cmd == Command.CMD_CLEAR_POPUPS)
        then
            Executor:clear_popups()
        end
        Command:finish() -- 任务执行完毕
        Runtime:sleep(100)
    end
    function Start()
        while (true)
        do
            local ok, err_msg = pcall(interpret)
            if (not ok)
            then
                Error:catch(err_msg --[[@as string]])
            end
        end
    end

end -- Start_lua
