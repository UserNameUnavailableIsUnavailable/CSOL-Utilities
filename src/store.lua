module = {}

----- 商店相关 -----

local config = require("config")

local STORE_PURCHASE_CHOICE_X = 22949
local STORE_PURCHASE_CHOICE_Y = 39479

local STORE_PURCHASE_X = 35858
local STORE_PURCHASE_Y = 42212

local STORE_PURCHASE_CONFIRM_X = 36712
local STORE_PURCHASE_CONFIRM_Y = 39357
function module.PurchaseItem(x, y)
    while (IsKeyLockOn("scrolllock"))
    do
        MoveMouseTo(x, y) -- 兑换按钮位置
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)

        MoveMouseTo(STORE_PURCHASE_CHOICE_X, STORE_PURCHASE_CHOICE_Y) -- 选项
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)

        MoveMouseTo(STORE_PURCHASE_X, STORE_PURCHASE_Y) -- 购买按钮
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)

        MoveMouseTo(STORE_PURCHASE_CONFIRM_X, STORE_PURCHASE_CONFIRM_Y) -- 确认
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.LONG_LONG_INTERVAL)

        PressAndReleaseKey("escape")

    end
end

return module