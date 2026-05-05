if not __DELAY_LUA__ then
    __DELAY_LUA__ = true
    local __version__ = "1.5.2"

    Include("Version.lua")
    Version:set("Delay", __version__)

    ---基础时延。
    ---@enum Delay
    Delay = {
        MINI_MINI = 5,
        MINI = 10,
        SHORT = 50,
        NORMAL = 100,
        MEDIUM = 150,
        LONG = 200,
        LONG_LONG = 500,
        REFRESH = 1000,
    }
end -- __DELAY_LUA__
