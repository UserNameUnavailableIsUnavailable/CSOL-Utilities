--[[
    config.lua 预设文件，保存按钮坐标位置、延迟设定等常量  
--]]

----- 游戏内按钮位置 -----
-- “开始游戏”按钮

module = {}

module.GAMESTART_X = 47025
module.GAMESTART_Y = 42030
-- 游戏结算页面的“确认”按钮
module.GAME_SETTLEMENT_CONFIRM_X = 44976
module.GAME_SETTLEMENT_CONFIRM_Y = 51748
-- ESC菜单的“取消”按钮
module.ESC_MENU_CANCEL_X = 32375
module.ESC_MENU_CANCEL_Y = 43913

-- 屏幕中央
module.SCREEN_MIDDLE_X = 32768
module.SCREEN_MIDDLE_Y = 32768
----- 延时设定 -----
module.MINI_INTERVAL = 10
module.SHORT_INTERVAL = 50
module.NORMAL_INTERVAL = 100
module.MEDIUM_INTERVAL = 150
module.LONG_INTERVAL = 200
module.LONG_LONG_INTERVAL = 500
module.REFRESH_INTERVAL = 1000

----- 鼠标按键 -----
module.LEFT_MOUSE_BUTTON = "1"
module.MIDDLE_MOUSE_BUTTON = "2"
module.RIGHT_MOUSE_BUTTON = "3"


return module