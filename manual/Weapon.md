# `Weapon` 类

`Weapon` 类用于定义挂机时使用的武器或装备。

## 字段

### `name`

`name` 标识武器或装备的名称，类型为字符串。

> 该字段默认值为 `"未知"`。

### `number`

`number` 成员用于标识武器或装备的栏位，可以取下列值：

|名称|语义|
|---|---|
|Weapon.NULL|表示不会被使用的武器或装备，如防弹衣、配件武器|
|Weapon.PRIMARY|主武器|
|Weapon.SECONDARY|副武器|
|Weapon.MELEE|近战武器|
|Weapon.GRENADE|手雷|

> 该字段默认值为 `Weapon.NULL`。

### `attack_button`

标识**常规**武器或装备的攻击按钮，可取如下值：

|名称|语义|
|---|---|
|Mouse.LEFT|鼠标左键|
|Mouse.RIGHT|鼠标右键|

> 该字段默认值为 `Mouse.LEFT`。

### `switch_delay`

标识武器或装备的切换延迟，单位为毫秒

> 该字段默认值为 `150`。

### `purchase_sequence`

记录武器或装备的购买按键序列，例如：

```lua
weapon.purchase_sequence = { Keyboard.B, Keyboard.J, Keyboard.EIGHT, Keyboard.SEVEN }
```

将 `weapon` 的购买按键序列设置为 `B` `J` `8` `7`。

> 该字段默认值为 `{}`。

### `RELOAD_KEY`

重新装填按键，该设定与配置面板中的提供的重新装填按钮设定选项保持一致。在使用“圣翼皓印”、“万钧神威”等定制武器时，特殊攻击将通过 `RELOAD_KEY` 触发。

> 该字段默认为 `Keyboard.R`。


## 函数

### `new`

构造新武器或装备。

`Weapon:new(obj)`

- `obj` 类型为 `table`，其中应包含 `Weapon` 对象的初始化列表。此外，还可以在其中添加一些自定义的字段。

#### 示例

下面的例子创建一件名为“幻境！光棱剑”的近战武器，购买按键序列为 `B` `G`，切换延迟为 100 *ms*，使用鼠标左键进行攻击。

```lua
Weapon:new{
    name = "幻境！光棱剑",
    number = Weapon.MELEE,
    purchase_sequence = { Keyboard.B, Keyboard.G },
    switch_delay = 100,
    attack_button = Mouse.LEFT,
}
```

### `purchase`

用于在挂机时购买指定的武器或装备。

`Weapon:purchase()`

### `switch`

切换至指定的武器或装备，并在切换后等待指定的切换延迟时间。

`Weapon:switch()`

### `switch_without_delay`

切换至指定的武器或装备，切换后忽略切换延迟时间。

### `abandon`

丢弃指定的武器。

`Weapon:abandon()`

### `attack`

使用指定的**常规**武器进行**一轮**攻击。

`Weapon:attack()`

> 该函数具有默认的实现。当需要定制常规武器攻击方式时，可重写此函数。在重写时需要注意执行一次 `attack` 的时间一般不超过 10 秒。

以下示例创建了定制武器万钧神威，重写了 `attack` 方法。

```lua
Weapon:new{
    name = "万钧神威",
    switch_delay = Delay.SHORT,
    number = Weapon.MELEE,
    purchase_sequence = {
        Keyboard.B,
        Keyboard.NINE,
        Keyboard.FOUR
    },
    attack = function (self)
        Mouse:press(Mouse.RIGHT)
        local sensitivity_x = 1 - 0.8 * math.random()
        local sensitivity_y = 1 - 0.8 * math.random()
        local direction = Utility:random_direction()
        local start_time = Runtime:get_running_time()
        local last_switch_time = 0
        local first_throw = false
        local second_throw = false
        repeat					local current_time = Runtime:get_running_time()
            if current_time - last_switch_time > 1000
            then
                self:switch_without_delay()
                last_switch_time = current_time
            end
            if not Mouse:move_relative(math.floor(direction * 100 * sensitivity_x), math.floor(math.sin(current_time / 1000) * 100 * sensitivity_y), Delay.MINI)
            then
                break
            end
            local duration = Runtime:get_running_time() - start_time
            if not first_throw and 3000 < duration and duration < 6000
            then
                Keyboard:click(Keyboard.R, Delay.SHORT)
                first_throw = true
            end
            if not second_throw and 6000 < duration
            then
                Keyboard:click(Keyboard.R, Delay.SHORT)
                second_throw = true
            end
        until Runtime:get_running_time() - start_time > 7000
        return Mouse:release(Mouse.RIGHT)
    end
}
```

### `use`

使用指定的**特殊**武器进行攻击。

`Weapon:use()`

> 该函数不提供默认的实现，故需要重写此函数以使用特殊武器进行攻击。在重写时需要注意执行一次 `use` 的时间一般不超过 10 秒。

以下示例创建了特殊武器圣翼皓印，并重写了 `use` 方法。需要注意，可以在初始化列表中增加一些自定义的字段（如例中的 `discharging` 等），并在 `use` 内使用它们。

```lua
Weapon:new{
    name = "圣翼皓印/炽翼魔印",
    switch_delay = 750,
    number = Weapon.GRENADE,
    purchase_sequence = {
        Keyboard.B,
        Keyboard.EIGHT,
        Keyboard.NINE
    },
    discharging = false,
    discharge_start_moment = 0,
    recharge_start_moment = 0,
    DISCHARGE_TIME = 25,
    RECHARGE_TIME = 10,
    use = function (self)
        local current_time = DateTime:get_local_timestamp()
        if not self.discharging and current_time - self.recharge_start_moment > self.RECHARGE_TIME
        then
            self:switch()
            Mouse:click(Mouse.LEFT, 200)
            self.discharging = true
            self.discharge_start_moment = current_time
        elseif self.discharging and current_time - self.discharge_start_moment > self.DISCHARGE_TIME
        then
            self:switch()
            Mouse:move_relative(0, 4000, Delay.NORMAL)
            Keyboard:click(Keyboard.R, 350)
            Mouse:move_relative(0, - 4000, Delay.NORMAL)
            self.discharging = false
            self.recharge_start_moment = current_time
        end
    end
}
```