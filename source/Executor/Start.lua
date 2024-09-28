if (not Start_lua)
then
Start_lua = true
---判断命令是否失效。
local function expired()
    CmdTime = CmdTime or 0
    local current_time = DateTime:get_local_timestamp() -- 本地时间戳
    return math.abs(current_time - CmdTime) > 2
end

function Start()
    local previous_command = Command.NOP -- 上一次获取到的命令，初始为 NOP
    while (true)
    do
        if (pcall(Include, "$~cmd.lua"))
        then
            Cmd = Cmd or Command.CMD_NOP
        else
            Cmd = Command.CMD_NOP
        end
        -- 文件中的命令时间戳与当前时间的差值达到 5 秒认为文件中的命令无效
        if (Cmd == Command.CMD_START_GAME_ROOM and not expired()) -- 开始游戏
        then
            Executor:start_game_room()
        elseif (Cmd == Command.CMD_CHOOSE_CLASS and not expired()) -- 选定角色
        then
            Executor:choose_class()
        elseif (Cmd == Command.CMD_PLAY_GAME_NORMAL and not expired()) -- 24H 挂机模式（常规）
        then
            Executor:try_confirm()
            Player:buy_part_weapon(PartWeaponList)
            Player:play(DefaultWeaponList)
        elseif (Cmd == Command.CMD_PLAY_GAME_EXTEND and not expired()) -- 24H 挂机模式（扩展）
        then
            Executor:try_confirm()
            Player:use_special_weapon(SpecialWeapon)
            Player:play(ExtendedWeaponList)
        elseif (Cmd == Command.CMD_CREATE_ROOM and not expired()) -- 创建房间功能
        then
            Executor:create_game_room()
        elseif (Cmd == Command.CMD_COMBINE_PARTS and not expired()) -- 合成配件功能
        then
            Executor:combine_parts()
        elseif (Cmd == Command.CMD_PURCHASE_ITEM and not expired()) -- 购买物品功能
        then
            if (previous_command ~= Cmd) -- 对于新发出的命令，需要更新鼠标光标位置
            then
                Executor:purchase_item(Mouse:locate_cursor())
            end
            Executor:purchase_item() -- 仍使用上次坐标
        elseif (Cmd == Command.CMD_LOCATE_CURSOR and not expired()) -- 光标定位功能
        then
            Executor:locate_cursor()
        end
        Runtime:sleep(50)
        previous_command = Cmd -- 更新上一次命令
    end
end
end -- Start_lua