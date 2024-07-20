-- 游戏内各种按钮坐标
if (not Setting_lua)
then
Setting_lua = true
Setting = {
    -- 操作系统时区，默认为东八区（UTC+8），此项必须设置正确，否则无法正常运行。
    TIME_ZONE = 8,
---游戏大厅---
    --- “房间列表”选框
    HALL_ROOM_LIST_X = 36644,
    HALL_ROOM_LIST_Y = 29640,
    ---大厅界面的“新建房间”按钮
    HALL_CREATE_ROOM_X = 38283,
    HALL_CREATE_ROOM_Y = 36746,
    ---房间列表界面的“返回”按钮。
    HALL_BACK_X = 27799,
    HALL_BACK_Y = 10750,
    ---“游戏模式”选框
    GAME_MODE_X = 33775,
    GAME_MODE_Y = 24720,
    ---“大灾变”模式选框
    ZOMBIE_SCENARIO_MODE_X = 42449,
    ZOMBIE_SCENARIO_MODE_Y = 22959,
    ---地图选择界面中的向左滚动按钮
    MAP_CHOOSE_LEFT_SCROLL_X = 27969,
    MAP_CHOOSE_LEFT_SCROLL_Y = 45249,
    ---“地狱围栏”灾变地图
    MAP_TRAP_X = 21788,
    MAP_TRAP_Y = 36017,
    ---“选择完成”按钮
    FINISH_CHOOSE_X = 44362,
    FINISH_CHOOSE_Y = 43670,
    ---“游戏难度”选框
    GAME_DIFFICULTY_X = 33126,
    GAME_DIFFICULTY_Y = 39114,
    ---难度选项（已废弃）
    -- GAME_DIFFICULTY_OPTION_X = 32921,
    -- GAME_DIFFICULTY_OPTION_Y = 42880,
    --- 难度选项（0 - 入门，1 普通，2 困难，以此类推）
    GAME_DIFFICULTY_OPTION = 2,
    ---“新建房间”按钮
    CREATE_ROOM_X = 30223,
    CREATE_ROOM_Y = 44945,
---游戏内---
    -- “开始游戏”按钮
    ROOM_START_GAME_X = 47025,
    ROOM_START_GAME_Y = 42030,
    -- 是否选择 T 阵营角色
    CHOOSE_T_CLASS = false,
    -- 角色选择界面 T 阵营
    CHOOSE_T_CLASS_X = 11304,
    CHOOSE_T_CLASS_Y = 9596,
    -- 角色选项
    CLASS_OPTION = 3, -- 可取 0 ~ 9
    -- ESC菜单的“取消”按钮
    ZS_GAME_ESC_MENU_CANCEL_X = 32375,
    ZS_GAME_ESC_MENU_CANCEL_Y = 43913,
    -- 【灾变】资金不足，无法购买
    GAME_INSUFFIENT_FUNDS_CONFIRM_X = 32750,
    GAME_INSUFFIENT_FUNDS_CONFIRM_Y = 34924,
    -- 【灾变】死亡状态预购买菜单“重复购买”按钮（不建议点击取消购买，会与大厅界面其他按钮位置冲突）
    GAME_DEAD_PURCHASE_MENU_REBUY_X = 31692,
    GAME_DEAD_PURCHASE_MENU_REBUY_Y = 43123,
    -- 游戏结算页面的“确认”按钮
    GAME_ROUND_CONFIRM_X = 44976,
    GAME_ROUND_CONFIRM_Y = 51748,
---制造所---
    -- 【制造所】自动添加配件按钮
    CRAFT_PARTS_AUTO_FILL_X = 20012,
    CRAFT_PARTS_AUTO_FILL_Y = 50533,
    -- 【制造所】配件“开始合成”按钮
    CRAFT_PARTS_COMBINE_X = 25989,
    CRAFT_PARTS_COMBINE_Y = 50654,
    -- 【制造所】配件“清空”按钮
    CRAFT_PARTS_CLEAR_X = 33263,
    CRAFT_PARTS_CLEAR_Y = 50594,
---商店---
    -- 【商店】点击兑换后，弹窗中的兑换物品选项
    STORE_BUY_OPTION_X = 22949,
    STORE_BUY_OPTION_Y = 39479,
    -- 【商店】物品“兑换”->“兑换”按钮（点击兑换后弹出界面上的“兑换”按钮）
    STORE_BUY_X = 36644,
    STORE_BUY_Y = 41726,
    -- 【商店】物品“兑换”->“兑换”->“确认”按钮
    STORE_BUY_CONFIRM_X = 37019,
    STORE_BUY_CONFIRM_Y = 38993,
}
end -- Setting_lua
