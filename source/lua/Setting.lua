-- 游戏内各种按钮坐标
if (not Setting_lua)
then
Setting_lua = true
Setting = {
    -- “开始游戏”按钮
    GAMESTART_X = 47025,
    GAMESTART_Y = 42030,
    -- 游戏结算页面的“确认”按钮
    GAME_SETTLEMENT_CONFIRM_X = 44976,
    GAME_SETTLEMENT_CONFIRM_Y = 51748,
    -- ESC菜单的“取消”按钮
    ESC_MENU_CANCEL_X = 32375,
    ESC_MENU_CANCEL_Y = 43913,
    -- 【灾变】资金不足，无法购买
    GAME_INSUFFIENT_FUNDS_CONFIRM_X = 32750,
    GAME_INSUFFIENT_FUNDS_CONFIRM_Y = 34924,
    -- 【灾变】死亡状态预购买菜单“重复购买”按钮（不建议点击取消购买，会与大厅界面其他按钮位置冲突）
    GAME_PREORDER_MENU_REBUY_X = 31692,
    GAME_PREORDER_MENU_REBUY_Y = 43123,
    -- 【制造所】自动添加配件按钮
    CRAFT_PARTS_AUTO_FILL_X = 20012,
    CRAFT_PARTS_AUTO_FILL_Y = 50533,
    -- 【制造所】配件“开始合成”按钮
    CRAFT_PARTS_COMBINE_X = 25989,
    CRAFT_PARTS_COMBINE_Y = 50654,
    -- 【制造所】配件“清空”按钮
    CRAFT_PARTS_CLEAR_X = 33263,
    CRAFT_PARTS_CLEAR_Y = 50594,
    -- 【商店】点击兑换后，弹窗中的兑换物品选项
    STORE_BUY_OPTION_X = 22949,
    STORE_BUY_OPTION_Y = 39479,
    -- 【商店】物品“兑换”->“兑换”按钮（点击兑换后弹出界面上的“兑换”按钮）
    STORE_BUY_X = 36644,
    STORE_BUY_Y = 41726,
    -- 【商店】物品“兑换”->“兑换”->“确认”按钮
    STORE_BUY_CONFIRM_X = 37019,
    STORE_BUY_CONFIRM_Y = 38993,
    -- 角色选择界面 T 阵营
    CHOOSE_T_CLASS_X = 11304,
    CHOOSE_T_CLASS_Y = 9596
}
end -- Setting_lua
