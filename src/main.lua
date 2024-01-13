local round = require("round")
local craft = require("craft")
local store = require("store")

function OnEvent(event, arg, family)
    local x, y
    if (IsKeyLockOn("scrolllock")) then
        if (event == "MOUSE_BUTTON_PRESSED" and arg == 7 and family == "mouse") then
            OutputLogMessage("\nThe script is loaded.\n")
            round.Rotate()
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 6 and family == "mouse") then
            x, y = GetMousePosition()
            OutputLogMessage("(%d, %d)\n", x, y)
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 5 and family == "mouse") then
            craft.CombineParts()
        elseif (event == "MOUSE_BUTTON_PRESSED" and arg == 4 and family == "mouse") then
            x, y = GetMousePosition()
            store.PurchaseItem(x, y)
        end
    end
end

