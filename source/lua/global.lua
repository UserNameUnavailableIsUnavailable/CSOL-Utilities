if (not Global_lua)
then
Global_lua = true
-- 按照依赖顺序导入各个模块
Load("DateTime.lua") -- 不依赖任何模块
Load("Console.lua") -- 不依赖任何模块
Load("Delay.lua") -- 不依赖任何模块
Load("Setting.lua") -- 不依赖任何模块
Load("Command.lua") -- 不依赖任何模块
Load("Context.lua") -- 不依赖任何模块
Load("Utility.lua") -- 不依赖任何模块
Load("Runtime.lua") -- 依赖于 `Context`
Load("Keyboard.lua") -- 依赖于 `Runtime`
Load("Mouse.lua") -- 依赖于 `Runtime`
Load("Weapon.lua") -- 依赖于上述模块
Load("WeaponList.lua") -- 依赖于上述模块
Load("Player.lua") -- 依赖于上述模块
Load("Executor.lua") -- 依赖于上述模块
Load("Interpreter.lua") -- 依赖于上述模块

end -- Global_lua