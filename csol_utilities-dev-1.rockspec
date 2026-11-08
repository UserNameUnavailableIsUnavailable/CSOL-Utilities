package = "csol_utilities"
version = "dev-1"
source = {
   url = "git+https://github.com/UserNameUnavailableIsUnavailable/CSOL-Utilities.git"
}
description = {
   detailed = "功能强大且高度自定义的 CSOL 集成工具，含 24 H 转圈（无需罗技设备）、商店批量购买、配件批量合成等功能。",
   homepage = "https://github.com/UserNameUnavailableIsUnavailable/CSOL-Utilities",
   license = "MIT"
}
dependencies = {
   "lua = 5.4",
   "luaunit"
}
build = {
   type = "builtin",
   modules = {
      ["executor.core.Exception"] = "executor/core/Exception.lua",
      ["executor.core.Promise"] = "executor/core/Promise.lua",
      ["executor.core.Scheduler"] = "executor/core/Scheduler.lua",
      ["executor.core.Timer"] = "executor/core/Timer.lua",
      ["testing.executor.Promise"] = "testing/executor/Promise.lua"
   }
}
