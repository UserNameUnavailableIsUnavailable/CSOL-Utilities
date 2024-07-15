if (not Global_lua)
then
Global_lua = true
---提前声明诸模块，防止产生依赖问题
Console = {}
Delay = {}
Setting = {}
Command = {}
Context = {}
Runtime = {}
Mouse = {}
Keyboard = {}
Executor = {}
Utility = {}
Player = {}
Weapon = {}
Load("DateTime.lua")
Load("Console.lua")
Load("Delay.lua")
Load("Setting.lua")
Load("Command.lua")
Load("Context.lua")
Load("Runtime.lua")
Load("Keyboard.lua")
Load("Mouse.lua")
Load("Executor.lua")
Load("Utility.lua")
Load("Weapon.lua")
Load("WeaponList.lua")
Load("Player.lua")
Load("Interpreter.lua")

end -- Global_lua