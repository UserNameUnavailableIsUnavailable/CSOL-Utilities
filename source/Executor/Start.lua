if (not Start_lua)
then
    Start_lua = true
    Include("Automation.lua")
    ---注册完所有中断处理函数后，开中断。
    Runtime:set_interrupt_flag() -- 开中断

    local function interpret()
        local cmd = Command:claim() -- 领取任务
        if (cmd == Command.CMD_START_GAME_ROOM) -- 开始游戏
        then
            Automation:start_game_room()
        elseif (cmd == Command.CMD_CHOOSE_CHARACTER) -- 选定角色
        then
            Automation:choose_character()
        elseif (cmd == Command.CMD_DEFAULT_IDLE) -- 24H 挂机模式（常规）
        then
            Automation:try_confirm()
            Automation:choose_golden_zombie_reward()
            Automation.default_player:play()
        elseif (cmd == Command.CMD_EXTENDED_IDLE) -- 24H 挂机模式（扩展）
        then
            Automation:try_confirm()
            Automation:choose_golden_zombie_reward()
            Automation.extended_player:play()
        elseif (cmd == Command.CMD_CREATE_GAME_ROOM) -- 创建房间功能
        then
            Automation:create_game_room()
        elseif (cmd == Command.CMD_BATCH_COMBINE_PARTS) -- 合成配件功能
        then
            Automation:combine_parts()
        elseif (cmd == Command.CMD_BATCH_PURCHASE_ITEM) -- 购买物品功能
        then
            -- 对于新发出的命令，需要更新鼠标光标位置
            if ((Command:get_status() & Command.NAME_CHANGED) ~= Command.UNCHANGED) -- 新旧命令类型不同，即旧命令不是购买物品
            then
                Automation:purchase_item(Mouse:locate())
            else
                Automation:purchase_item()
            end
        elseif (cmd == Command.CMD_LOCATE_CURSOR) -- 光标定位功能
        then
            Automation:locate_cursor()
        elseif (cmd == Command.CMD_CLEAR_POPUPS)
        then
            Automation:clear_popups()
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
