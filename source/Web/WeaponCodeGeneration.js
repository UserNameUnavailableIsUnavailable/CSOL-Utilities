function GeneratePartWeaponCode() {
    let name = document.getElementById("part_weapon_name").value;
    if (!IsFieldValid(name)) { alert("武器名称未配置！"); }
    let purchase_sequence = document.getElementById("part_weapon_purchase_sequence").value;
    if (!IsFieldValid(purchase_sequence)) { alert("购买按键序列未配置！"); }
    purchase_sequence = ResolvePurchaseSequence(purchase_sequence);
    let lua_snippet = "Weapon:new{\r\n" +
        `\tname = \"${name}\",\r\n` +
        `\tpurchase_sequence = ${purchase_sequence},\r\n` +
        "}";
    document.getElementById("part_weapon_code").value = lua_snippet;
}

function GenerateNormalWeaponCode() {
    let customized_normal_weapon = document.querySelector("input[name='customized_normal_weapon']:checked").value == "true" ? true : false;
    if (customized_normal_weapon) {
        let customized_normal_weapon_option = document.getElementById("customized_normal_weapon_option").value;
        let purchase_sequence = document.getElementById("customized_normal_weapon_purchase_sequence").value;
        if (!IsFieldValid(purchase_sequence)) { alert("购买按键序列未配置！"); }
        purchase_sequence = ResolvePurchaseSequence(purchase_sequence);
        let lua_snippet = "";
        switch (customized_normal_weapon_option) {
            case "twin_axis": lua_snippet = twin_axis.replace("{}--[[purchase_sequence]]", purchase_sequence); break;
            case "brionac": lua_snippet = brionac.replace("{}--[[purchase_sequence]]", purchase_sequence); break;
        }
        document.getElementById("normal_weapon_code").value = lua_snippet;
        return;
    }
    let name = document.getElementById("normal_weapon_name").value;
    if (!IsFieldValid(name)) { alert("武器名称未配置！"); }
    let purchase_sequence = document.getElementById("normal_weapon_purchase_sequence").value;
    if (!IsFieldValid(purchase_sequence)) { alert("购买按键序列未配置！"); }
    purchase_sequence = ResolvePurchaseSequence(purchase_sequence);
    let number = document.getElementById("normal_weapon_number").value;
    let attack_button = document.getElementById("normal_weapon_attack_button").value;
    let switch_delay = document.getElementById("normal_weapon_switch_delay").value;
    switch_delay = (!IsFieldValid(switch_delay)) ? "Delay.NORMAL" : switch_delay;
    let lua_snippet = "Weapon:new{\r\n" +
        `\tname = \"${name}\",\r\n` +
        `\tpurchase_sequence = ${purchase_sequence},\r\n` +
        `\tnumber = ${number},\r\n` +
        `\tattack_button = ${attack_button},\r\n` +
        `\tswitch_delay = ${switch_delay}\r\n` +
        "}";
    document.getElementById("normal_weapon_code").value = lua_snippet;
}

function GenerateSpecialWeaponCode() {
    let lua_snippet = "";
    let special_weapon_option = document.getElementById("special_weapon_option").value;
    let purchase_sequence = document.getElementById("special_weapon_purchase_sequence").value;
    if (!IsFieldValid(purchase_sequence)) { alert("购买按键序列未配置！"); }
    purchase_sequence = ResolvePurchaseSequence(purchase_sequence);
    switch (special_weapon_option) {
        case "sandalphon_or_belial": lua_snippet = sandalphon_or_belial.replace("{}--[[purchase_sequence]]", purchase_sequence); break;
        case "hellhound": lua_snippet = hellhound.replace("{}--[[purchase_sequence]]", purchase_sequence); break;
    }
    document.getElementById("special_weapon_code").value = lua_snippet;
}

const NUMBERS = {
    '0': "Keyboard.ZERO",
    '1': "Keyboard.ONE",
    '2': "Keyboard.TWO",
    '3': "Keyboard.THREE",
    '4': "Keyboard.FOUR",
    '5': "Keyboard.FIVE",
    '6': "Keyboard.SIX",
    '7': "Keyboard.SEVEN",
    '8': "Keyboard.EIGHT",
    '9': "Keyboard.NINE"
}
function ResolvePurchaseSequence(sequence) {
    let purchase_sequence = "{";
    sequence = sequence.toUpperCase();
    for (let i = 0; i < sequence.length; i++) {
        j = sequence[i];
        if (j >= '0' && j <= '9') {
            purchase_sequence += NUMBERS[j];
        }
        else if (j >= 'A' && j <= 'Z') {
            purchase_sequence += "Keyboard." + j;
        }
        else {
            continue;
        }
        if (i != sequence.length - 1) {
            purchase_sequence += ", ";
        }
    }
    purchase_sequence += "}";
    return purchase_sequence;
}

function IsFieldValid(field) {
    return field != undefined && field != "";
}

var twin_axis =
    `Weapon:new{
    name = "神鬼开天/魔神开天",
    switch_delay = Delay.SHORT,
    number = Weapon.MELEE,
    purchase_sequence = {}--[[purchase_sequence]],
    -- 重写 attack 方法，按照下面定义的方式进行攻击
    -- 若您使用神鬼开天/魔神开天进行挂机且没有编程经验，则请勿修改此函数
    attack = function (self)
        Mouse:press(Mouse.RIGHT)
        local sensitivity_x = 1 - 0.8 * math.random() -- 水平灵敏度∈(0.2, 1]
        local sensitivity_y = 1 - 0.8 * math.random() -- 竖直灵敏度∈(0.2, 1]
        local direction = Utility:random_direction() -- 随机向左或右
        local start_time = Runtime:get_running_time() -- 本次转圈开始时间
        repeat
            local duration = Runtime:get_running_time() - start_time
            local t = Runtime:get_running_time() / 1000
            Mouse:move_relative(math.floor(direction * 100 * sensitivity_x), math.floor(math.sin(t) * 100 * sensitivity_y), Delay.MINI) -- 视角运动：水平方向匀速运动，竖直方向简谐运动
        until (Runtime:get_running_time() - start_time > 6000)
        Mouse:release(Mouse.RIGHT, 200) -- 松开鼠标右键释放旋风
        Mouse:press(Mouse.LEFT, 1000)
        Keyboard:press(Keyboard.R, 200)
        Keyboard:release(Keyboard.R)
        Mouse:release(Mouse.LEFT)
    end
}`;

var brionac =
    `Weapon:new{
    name = "万钧神威",
    switch_delay = Delay.SHORT,
    number = Weapon.MELEE,
    purchase_sequence = {}--[[purchase_sequence]],
    -- 重写 attack 方法，按照下面定义的方式进行攻击
    -- 若您需要使用万钧神威进行挂机且没有编程经验，则请勿修改此函数
    attack = function (self)
        Mouse:press(Mouse.RIGHT) -- 按下鼠标右键进行范围攻击
        local sensitivity_x = 1 - 0.8 * math.random() -- 水平灵敏度∈(0.2, 1]
        local sensitivity_y = 1 - 0.8 * math.random() -- 竖直灵敏度∈(0.2, 1]
        local direction = Utility:random_direction() -- 随机向左或右
        local start_time = Runtime:get_running_time() -- 本次转圈开始时间
        local first_throw = false
        local second_throw = false
        repeat
            local duration = Runtime:get_running_time() - start_time
            local t = Runtime:get_running_time() / 1000
            Mouse:move_relative(math.floor(direction * 100 * sensitivity_x), math.floor(math.sin(t) * 100 * sensitivity_y), Delay.MINI) -- 视角运动：水平方向匀速运动，竖直方向简谐运动
            if (not first_throw and 3000 < duration and duration < 6000)
            then
                Keyboard:click(Keyboard.R, Delay.SHORT)
                first_throw = true
            end
            if (not second_throw and 6000 < duration)
            then
                Keyboard:click(Keyboard.R, Delay.SHORT)
                second_throw = true
            end
        until (Runtime:get_running_time() - start_time > 7000)
        Mouse:release(Mouse.RIGHT) -- 松开鼠标右键释放旋风
    end
}`;

var sandalphon_or_belial =
    `Weapon:new{
    name = "圣翼皓印/炽翼魔印",
    switch_delay = Delay.LONG_LONG,
    number = Weapon.GRENADE ,
    purchase_sequence = {Keyboard.B, Keyboard.EIGHT, Keyboard.NINE},
    discharging = false, -- 是否在释放光印
    discharge_start_moment = 0, --  光印释放的时刻。
    charge_start_moment = 0, -- 充能开始的时刻。
    DISCHARGE_TIME = 25, -- 光印释放时间。
    RECHARGE_TIME = 10, -- 充能时间。
    use = function (self)
        local current_time = DateTime:get_local_timestamp() -- 当前时间戳
        -- 当前正在充能，且充能时间超过 \`RECHARGE_TIME\`。
        if (not self.discharging and current_time - self.charge_start_moment > self.RECHARGE_TIME)
        then
            self.discharging = true
            self.discharge_start_moment = current_time
            self:switch()
            Mouse:click(Mouse.LEFT, 200)
        -- 当前正在释放，且释放时间超过 \`DISCHARGE_TIME\`。
        elseif (self.discharging and current_time - self.charge_start_moment > self.DISCHARGE_TIME)
        then
            self.discharging = false
            self.charge_start_moment = current_time
            self:switch()
            -- 按 \`Keyboard.R\` 召唤界徐圣。 
            Mouse:move_relative(0, 4000, Delay.NORMAL)
            Keyboard:click(Keyboard.R, 350)
            Mouse:move_relative(0, -4000, Delay.NORMAL)
        end
    end
}`;

var hellhound =
    `Weapon:new{
    name = "【幽浮】控制核心",
    purchase_sequence = {}--[[purchase_sequence]],
    switch_delay = 500,
    number = Weapon.GRENADE,
    COOL_DOWN_TIME = 60,
    last_start_moment = 0,
    use = function (self)
        local current_time = DateTime:get_local_timestamp()
        if (current_time - self.last_start_moment > self.COOL_DOWN_TIME)
        then
            self:switch()
            self.last_start_moment = current_time
            Mouse:click(Mouse.LEFT, 500)
        end
    end
}`;