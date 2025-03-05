-- Include 定义在 Main.lua 中
Include("DateTime.lua")
Include("Console.lua")
Include("Delay.lua")
Include("Command.lua")
Include("Context.lua")
Include("Utility.lua")
Include("Runtime.lua")
Include("Keyboard.lua")
Include("Mouse.lua")
Include("Weapon.lua")
Include("Player.lua")
Include("Executor.lua")
-- 导入用户配置
Include("Setting.lua")
Include("WeaponList.lua")
-- 初始化
DateTime:set_time_zone(Setting.FIELD_TIME_ZONE) -- 时区
Weapon:set_reload_key(Setting.KEYSTROKES_GAME_WEAPON_RELOAD_KEY[1]) -- 换弹按键
Player:set_respawn_key(Setting.KEYSTROKES_GAME_WEAPON_RELOAD_KEY[1]) -- 复活按键
Runtime:sti() -- 开中断
Include("Start.lua")