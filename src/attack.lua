----- 攻击相关 -----
function start_attack()
    PressMouseButton(attack_mouse_button) -- 开始攻击
    Sleep(SHORT_INTERVAL)
end

function stop_attack()
    ReleaseMouseButton(attack_mouse_button) -- 结束攻击
    Sleep(SHORT_INTERVAL)
end