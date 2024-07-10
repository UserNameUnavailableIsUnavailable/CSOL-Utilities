if (not Global_lua)
then
Global_lua = true
---提前声明诸模块，防止产生依赖问题
Console = Console or {}
Delay = Delay or {}
Setting = Setting or {}
Command = Command or {}
Context = Context or {}
Runtime = Runtime or {}
Mouse = Mouse or {}
Keyboard = Keyboard or {}
Executor = Executor or {}
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
end -- Global_lua