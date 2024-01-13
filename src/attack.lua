----- 攻击相关 -----
local weapon = require("weapon")
local config = require("config")
module = {}
function module.StartAttack()
    PressMouseButton(weapon.G_AttackButton) -- 开始攻击
    Sleep(config.SHORT_INTERVAL)
end

function module.StopAttack()
    ReleaseMouseButton(weapon.G_AttackButton) -- 结束攻击
    Sleep(config.SHORT_INTERVAL)
end

return module