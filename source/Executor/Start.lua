if (not Start_lua)
then
Start_lua = true
---判断命令是否失效。
local function valid()
    CmdTimepoint = CmdTimepoint or 0
    local current_time = DateTime:get_local_timestamp() -- 本地时间戳
    return math.abs(current_time - CmdTimepoint) < 5 and not Runtime:is_paused() -- 命令未失效、未即时暂停
end

function Start()
    local previous_command = Command.NOP -- 上一次获取到的命令，初始为 NOP
    while (true)
    do
        if (pcall(Include, "$~cmd.lua"))
        then
            CmdType = CmdType or Command.CMD_NOP
        else
            CmdType = Command.CMD_NOP
        end
        -- 文件中的命令时间戳与当前时间的差值达到 5 秒认为文件中的命令无效
        if (CmdType == Command.CMD_START_GAME_ROOM and valid()) -- 开始游戏
        then
            Executor:start_game_room()
        elseif (CmdType == Command.CMD_CHOOSE_CLASS and valid()) -- 选定角色
        then
            Executor:choose_class()
        elseif (CmdType == Command.CMD_PLAY_GAME_NORMAL and valid()) -- 24H 挂机模式（常规）
        then
            Executor:try_confirm()
            Player:buy_part_weapon(PartWeaponList)
            Player:play(DefaultWeaponList)
        elseif (CmdType == Command.CMD_PLAY_GAME_EXTEND and valid()) -- 24H 挂机模式（扩展）
        then
            Executor:try_confirm()
            Player:use_special_weapon(SpecialWeapon)
            Player:play(ExtendedWeaponList)
        elseif (CmdType == Command.CMD_CREATE_ROOM and valid()) -- 创建房间功能
        then
            Executor:create_game_room()
        elseif (CmdType == Command.CMD_COMBINE_PARTS and valid()) -- 合成配件功能
        then
            Executor:combine_parts()
        elseif (CmdType == Command.CMD_PURCHASE_ITEM and valid()) -- 购买物品功能
        then
            if (previous_command ~= CmdType) -- 对于新发出的命令，需要更新鼠标光标位置
            then
                Executor:purchase_item(Mouse:locate_cursor())
            end
            Executor:purchase_item() -- 仍使用上次坐标
        elseif (CmdType == Command.CMD_LOCATE_CURSOR and valid()) -- 光标定位功能
        then
            Executor:locate_cursor()
        elseif (CmdType == Command.CMD_CLEAR_POPUPS and valid())
        then
            Executor:clear_popups()
        end
        Runtime:sleep(50)
        previous_command = CmdType -- 更新上一次命令
    end
end
end -- Start_lua