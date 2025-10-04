if not WeaponList_lua then
    WeaponList_lua = true
    Include("Version.lua")
    Version:set("WeaponList", "1.5.4")
    Version:require("WeaponList", "Setting", "1.5.4")
    Version:require("WeaponList", "Weapon", "1.5.4")
    DefaultPartWeapons = {}

    DefaultConventionalWeapons = {}

    DefaultSpecialWeapons = {}

    ExtendedPartWeapons = {}

    ExtendedConventionalWeapons = {}

    ExtendedSpecialWeapons = {}
end
