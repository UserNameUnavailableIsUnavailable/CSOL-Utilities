--[[
    config.lua 预设文件，保存按钮坐标位置、延迟设定等常量  
--]]

----- 游戏内按钮位置 -----
-- “开始游戏”按钮
GAMESTART_X = 47025
GAMESTART_Y = 42030
-- 游戏结算页面的“确认”按钮
CONFIRM_X = 44976
CONFIRM_Y = 51748
-- ESC菜单的“取消”按钮
ESC_MENU_CANCEL_X = 32375
ESC_MENU_CANCEL_Y = 43913
-- 制造所的“自动添加按钮”
AUTO_FILL_X = 20012
AUTO_FILL_Y = 50533
-- 制造所的“开始合成”按钮
COMBINE_X = 25989
COMBINE_Y = 50654
--制造所配件合成的“重置”按钮
RESET_X = 33263
RESET_Y = 50594

----- 延时设定 -----
MINI_INTERVAL = 10
SHORT_INTERVAL = 50
NORMAL_INTERVAL = 100
MEDIUM_INTERVAL = 150
LONG_INTERVAL = 200
LONG_LONG_INTERVAL = 500
REFRESH_INTERVAL = 1000

----- 鼠标按键 -----
LEFT_MOUSE_BUTTON = "1"
MIDDLE_MOUSE_BUTTON = "2"
RIGHT_MOUSE_BUTTON = "3"

----- 轮次时间设定 -----
ROUND_TIME = 60 * 1000 -- 一轮时间为60秒（不是回合时间，而是程序的重置时间）
