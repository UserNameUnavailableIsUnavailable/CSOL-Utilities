utility = {}

function utility.ClickButton(x, y)
    MoveMouseTo(x, y)
    Sleep(config.SHORT_INTERVAL)
    PressAndReleaseMouseButton(config.LEFT_MOUSE_BUTTON)
    Sleep(config.SHORT_INTERVAL)
end

function utility.