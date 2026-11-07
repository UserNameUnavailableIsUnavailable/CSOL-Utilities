package = "CSOL-Utilities"
version = "dev-1"
source = {
   url = "git+https://github.com/UserNameUnavailableIsUnavailable/CSOL-Utilities.git"
}
description = {
   detailed = "功能强大且高度自定义的 CSOL 集成工具，含 24 H 转圈（无需罗技设备）、商店批量购买、配件批量合成等功能。",
   homepage = "https://github.com/UserNameUnavailableIsUnavailable/CSOL-Utilities",
   license = "*** please specify a license ***"
}
dependencies = {
   "lua = 5.4",
   "luaunit"
}

build = {
   type = "builtin",
   modules = {
      ["executor.bak.core"] = "executor.bak/core.lua",
      ["executor.bak.core.Automation"] = "executor.bak/core/Automation.lua",
      ["executor.bak.core.Command"] = "executor.bak/core/Command.lua",
      ["executor.bak.core.Console"] = "executor.bak/core/Console.lua",
      ["executor.bak.core.Constants"] = "executor.bak/core/Constants.lua",
      ["executor.bak.core.Context"] = "executor.bak/core/Context.lua",
      ["executor.bak.core.DateTime"] = "executor.bak/core/DateTime.lua",
      ["executor.bak.core.Delay"] = "executor.bak/core/Delay.lua",
      ["executor.bak.core.Directives"] = "executor.bak/core/Directives.lua",
      ["executor.bak.core.Emulator"] = "executor.bak/core/Emulator.lua",
      ["executor.bak.core.Event"] = "executor.bak/core/Event.lua",
      ["executor.bak.core.Exception"] = "executor.bak/core/Exception.lua",
      ["executor.bak.core.Global.d"] = "executor.bak/core/Global.d.lua",
      ["executor.bak.core.Hook"] = "executor.bak/core/Hook.lua",
      ["executor.bak.core.JSON"] = "executor.bak/core/JSON.lua",
      ["executor.bak.core.Keyboard"] = "executor.bak/core/Keyboard.lua",
      ["executor.bak.core.Main"] = "executor.bak/core/Main.lua",
      ["executor.bak.core.Mouse"] = "executor.bak/core/Mouse.lua",
      ["executor.bak.core.Player"] = "executor.bak/core/Player.lua",
      ["executor.bak.core.Routine"] = "executor.bak/core/Routine.lua",
      ["executor.bak.core.Runtime"] = "executor.bak/core/Runtime.lua",
      ["executor.bak.core.Utility"] = "executor.bak/core/Utility.lua",
      ["executor.bak.core.Version"] = "executor.bak/core/Version.lua",
      ["executor.bak.core.Weapon"] = "executor.bak/core/Weapon.lua",
      ["executor.bak.custom.Setting"] = "executor.bak/custom/Setting.lua",
      ["executor.bak.custom.WeaponList"] = "executor.bak/custom/WeaponList.lua",
      ["executor.bak.test.foo"] = "executor.bak/test/foo.lua",
      ["executor.core.Exception"] = "executor/core/Exception.lua",
      ["executor.core.Promise"] = "executor/core/Promise.lua",
      ["executor.core.Scheduler"] = "executor/core/Scheduler.lua",
      ["executor.core.Timer"] = "executor/core/Timer.lua",
      ["executor.core.Util"] = "executor/core/Util.lua",
      ["lghubex.source.dll.dll"] = {
         sources = "lghubex/source/dll/dll.c"
      },
      ["lghubex.source.dll.features"] = {
         sources = "lghubex/source/dll/features.c"
      },
      ["lghubex.source.dll.hook"] = {
         sources = "lghubex/source/dll/hook.c"
      },
      ["lghubex.source.dll.pattern"] = {
         sources = "lghubex/source/dll/pattern.c"
      },
      ["panel.public.weapon_templates.Belial"] = "panel/public/weapon_templates/Belial.lua",
      ["panel.public.weapon_templates.Brionac"] = "panel/public/weapon_templates/Brionac.lua",
      ["panel.public.weapon_templates.Hellhound"] = "panel/public/weapon_templates/Hellhound.lua",
      ["panel.public.weapon_templates.Lycaon"] = "panel/public/weapon_templates/Lycaon.lua",
      ["panel.public.weapon_templates.Sandalphon"] = "panel/public/weapon_templates/Sandalphon.lua",
      ["panel.public.weapon_templates.TwinLightAxes"] = "panel/public/weapon_templates/TwinLightAxes.lua",
      ["panel.public.weapon_templates.TwinShadowAxes"] = "panel/public/weapon_templates/TwinShadowAxes.lua",
      ["panel.src-tauri.src.data.Executor"] = "panel/src-tauri/src/data/Executor.lua",
      ["testing.executor.Promise"] = "testing/executor/Promise.lua"
   }
}
