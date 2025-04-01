if (not Delay_lua)
then
    Delay_lua = true
    Include("Version.lua")
    Version:set("Delay", { 1, 5, 1 })
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
end -- Delay_lua