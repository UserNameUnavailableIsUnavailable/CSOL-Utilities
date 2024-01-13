module = {}

local config = require("config")

----- 行走相关设定 -----
local walk_direction = 1 -- 0表示向前，1表示向后
local strafe_direction = 1 -- 0表示向右，1表示向左
local yaw_direction = 1 -- 0表示视角右转，1表示左转
local pitch_direction = 1 -- 0表示视角向下，1表示视角向上

local YAW_SPEED = 70
local PITCH_SPEED = 2

local time_array = {100, 200, 300, 400, 500}
local direction = {1, 1, 1, 1, 1}

local function ChangeDirection()
    time_array[5] = GetRunningTime()
    for i = 1, #time_array - 1
    do
        time_array[i] = time_array[i + 1]
    end
    for i = 1, #direction
    do
        math.randomseed(time_array[i])
        direction[i] = math.random(0, 1)
    end
    walk_direction = direction[1]
    strafe_direction = direction[2]
    yaw_direction = direction[3]
    pitch_direction = direction[4]
end

function module.StartWalk()
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

function module.StopWalk()
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
    ChangeDirection()
end

function module.Turn()
    local yaw_coeff
    local pitch_coeff
    math.randomseed(GetRunningTime())
    yaw_direction = math.random(0, 1)
    PressAndReleaseKey("r") -- 回合重置
    Sleep(config.SHORT_INTERVAL)
    -- 控制转向
    if (yaw_direction == 0)
    then
        yaw_coeff = 1
    else
        yaw_coeff = -1
    end

    if (pitch_direction == 0)
    then
        pitch_coeff = 1
    else
        pitch_coeff = -1
    end
    for i = 1, 400 do
        MoveMouseRelative(yaw_coeff * YAW_SPEED, pitch_coeff * PITCH_SPEED) -- 随机向左或向右转
        Sleep(config.MINI_INTERVAL)
    end
end

return module