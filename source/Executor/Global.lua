if (not Global_lua)
then
Global_lua = true
-- 按照依赖顺序导入各个模块
Include("DateTime.lua") -- 不依赖任何模块
Include("Console.lua") -- 不依赖任何模块
Include("Delay.lua") -- 不依赖任何模块
Include("Setting.lua") -- 不依赖任何模块
Include("Command.lua") -- 不依赖任何模块
Include("Context.lua") -- 不依赖任何模块
Include("Utility.lua") -- 不依赖任何模块
Include("Runtime.lua") -- 依赖于 `Context`
Include("Keyboard.lua") -- 依赖于 `Runtime`
Include("Mouse.lua") -- 依赖于 `Runtime`
Include("Weapon.lua") -- 依赖于上述模块
Include("WeaponList.lua") -- 依赖于上述模块
Include("Player.lua") -- 依赖于上述模块
Include("Executor.lua") -- 依赖于上述模块
Include("Start.lua") -- 依赖于上述模块

end -- Global_lua