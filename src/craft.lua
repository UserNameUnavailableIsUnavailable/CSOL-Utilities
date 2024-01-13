module = {}
----- 制造所相关 -----
-- 制造所的“自动添加按钮”
local CRAFT_PARTS_AUTO_FILL_X = 20012
local CRAFT_PARTS_AUTO_FILL_Y = 50533
-- 制造所的“开始合成”按钮
local CRAFT_PARTS_COMBINE_X = 25989
local CRAFT_PARTS_COMBINE_Y = 50654
--制造所配件合成的“重置”按钮
local CRAFT_PARTS_RESET_X = 33263
local CRAFT_PARTS_RESET_Y = 50594

local config = require("config")

----- 合成配件 -----
function module.CombineParts()
    local counter = 0
    while (IsKeyLockOn("scrolllock"))
    do
        PressKey("enter")
        MoveMouseTo(CRAFT_PARTS_AUTO_FILL_X, CRAFT_PARTS_AUTO_FILL_Y)
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)
        MoveMouseTo(CRAFT_PARTS_COMBINE_X, CRAFT_PARTS_COMBINE_Y)
        PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
        Sleep(config.MINI_INTERVAL)
        counter = counter + 1;
        if (counter > 30) then
            MoveMouseTo(CRAFT_PARTS_RESET_X, CRAFT_PARTS_RESET_Y)
            PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
            counter = 0
            Sleep(config.MINI_INTERVAL)
        end
        ReleaseKey("enter")
    end
end

return module