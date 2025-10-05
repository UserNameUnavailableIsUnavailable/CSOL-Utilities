if not __WEAPON_LIST_LUA__ then
    __WEAPON_LIST_LUA__ = true
    local __version__ = "1.5.4"

    Include("Version.lua")
    Version:set("WeaponList", __version__)
    Version:require("WeaponList", "Setting", __version__)
    Version:require("WeaponList", "Weapon", __version__)
    
    Armor =
        Weapon:new({ name = "护甲", })

    DefaultPartWeapons = {
        Weapon:new({ name = "默认武器列表 — 配件武器 1" }),
        Weapon:new({ name = "默认武器列表 — 配件武器 2" })
    }

    DefaultConventionalWeapons = {
        Weapon:new({ name = "默认武器列表 — 常规武器 1" }),
        Weapon:new({ name = "默认武器列表 — 常规武器 2" })
    }

    DefaultSpecialWeapons = {
        Weapon:new({ name = "默认武器列表 — 特殊武器 1" }),
        Weapon:new({ name = "默认武器列表 — 特殊武器 2" })
    }

    ExtendedPartWeapons = {
        Weapon:new({ name = "扩展武器列表 — 配件武器 1" }),
        Weapon:new({ name = "扩展武器列表 — 配件武器 2" })
    }

    ExtendedConventionalWeapons = {
        Weapon:new({ name = "扩展武器列表 — 常规武器 1" }),
        Weapon:new({ name = "扩展武器列表 — 常规武器 2" })
    }

    ExtendedSpecialWeapons = {
        Weapon:new({ name = "扩展武器列表 — 特殊武器 1" }),
        Weapon:new({ name = "扩展武器列表 — 特殊武器 2" })
    }
end -- __WEAPON_LIST_LUA__