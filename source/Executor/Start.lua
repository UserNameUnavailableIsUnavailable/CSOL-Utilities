if (not Start_lua)
then
Start_lua = true
---最近一次执行的命令 id，用于防止重复执行
local last_command_id = 0
-- 上一次获取到的命令，初始为 NOP
local last_command = Command.NOP
---判断命令是否失效。
local function valid()
    local current_time = DateTime:get_local_timestamp() -- 本地时间戳
    local expired = math.abs(current_time - CmdTimepoint) > 5 -- 命令是否过期
    local repeatable = CmdRepeatable -- 命令是否为可重复类型
    local changed = CmdId == last_command_id -- 命令是否改变
    local paused = Runtime:is_paused() -- 是否暂停

    -- 命令过期或者处于暂停状态
    if expired or paused
    then
        return false
	end
    -- 命令未过期，且当前没有暂停
    if not repeatable and not changed
    then
        return false
	end
    return true
end

function Start()
    while (true)
    do
        pcall(Include, "$~cmd.lua") -- 若读取命令文件失败，则仍保留上次的命令不变
		CmdId = CmdId or 0
		CmdType = CmdType or Command.CMD_NOP
		CmdTimepoint = CmdTimepoint or 0
		-- CmdRepeatable 无需确认是否为 nil，nil 与 false 等价
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
            if (last_command ~= CmdType) -- 对于新发出的命令，需要更新鼠标光标位置
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
        last_command_id = CmdId -- 更新最近一次执行的命令 Id
        last_command = CmdType -- 更新上一次命令内容
    end
end
end -- Start_lua
