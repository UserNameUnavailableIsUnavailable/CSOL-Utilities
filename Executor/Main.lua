if not Main_lua then
    Main_lua = true
    Include("Automation.lua")
    Include("Version.lua")
    assert(
        _VERSION >= "Lua 5.4",
        ([[当前 Lua 环境版本为 %s，执行器需要 Lua 5.4 及以上版本，请确保使用最新版本的 Logitech G Hub。]]):format(
            _VERSION
        )
    )
    Version:set("Main", "1.5.2")
    Version:assert()
    ---注册完所有中断处理函数后，开中断。
    Runtime:enable_interrupt() -- 开中断
    local function interpret()
        local cmd = Command:claim() -- 领取任务
        if
            cmd == Command.CMD_START_GAME_ROOM -- 开始游戏
        then
            Automation:start_game_room()
        elseif
            cmd == Command.CMD_CHOOSE_CHARACTER -- 选定角色
        then
            Automation:choose_character()
        elseif
            cmd == Command.CMD_DEFAULT_IDLE -- 24H 挂机模式（常规）
        then
            Automation:try_confirm()
            Automation.default_player:play()
        elseif
            cmd == Command.CMD_EXTENDED_IDLE -- 24H 挂机模式（扩展）
        then
            Automation:try_confirm()
            Automation.extended_player:play()
        elseif
            cmd == Command.CMD_CREATE_GAME_ROOM -- 创建房间功能
        then
            Automation:create_game_room()
        elseif
            cmd == Command.CMD_BATCH_COMBINE_PARTS -- 合成配件功能
        then
            Automation:combine_parts()
        elseif
            cmd == Command.CMD_BATCH_PURCHASE_ITEM -- 购买物品功能
        then
            -- 对于新发出的命令，需要更新鼠标光标位置
            if
                (Command:get_status() & Command.TYPE_CHANGED)
                ~= Command.UNCHANGED -- 新旧命令类型不同，即旧命令不是购买物品
            then
                Automation:purchase_item(Mouse:locate())
            else
                Automation:purchase_item()
            end
        elseif
            cmd == Command.CMD_LOCATE_CURSOR -- 光标定位功能
        then
            Automation:locate_cursor()
        elseif cmd == Command.CMD_CLEAR_POPUPS then
            Automation:clear_popups()
        end
        Command:finish() -- 任务执行完毕
        Runtime:sleep(100)
    end

    function Main()
        while Runtime:runnable() do
            local ok, err_msg = pcall(interpret)
            if not ok then
                Error:catch(err_msg --[[@as string]])
            end
        end
    end
end -- Main_lua
