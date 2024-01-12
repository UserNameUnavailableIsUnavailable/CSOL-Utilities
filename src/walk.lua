----- 行走相关设定 -----
local isWalking = false
local walk_direction = 1 -- 0表示向前，1表示向后
local strafe_direction = 1 -- 0表示向右，1表示向左

local function change_direction()
    math.randomseed(GetRunningTime())
    walk_direction = math.random(0, 1)
    strafe_direction = math.random(0, 1)
end

function start_walk()
    if (walk_direction == 0)
    then
        PressKey("w")
    else
        PressKey("s")
    end
    if (strafe_direction == 0)
    then
        PressKey("d")
    else
        PressKey("a")
    end
end

function stop_walk()
    if (walk_direction == 0)
    then
        ReleaseKey("w")
    else
        ReleaseKey("s")
    end
    if (strafe_direction == 0)
    then
        ReleaseKey("d")
    else
        ReleaseKey("a")
    end
    change_direction()
end
