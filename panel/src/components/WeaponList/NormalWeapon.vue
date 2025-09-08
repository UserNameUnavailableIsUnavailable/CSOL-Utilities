<!-- ConventionalWeapon 使用的配置控件 -->
<script lang="ts" setup>

import { ref, watch } from 'vue';
import CodeSnippet from '../CodeSnippet.vue';
import { GenerateWeaponCode } from '../../scripts/Weapon';
import BasicField from '../BasicField.vue';
import BasicKeystrokes from '../BasicKeystrokes.vue';
import BasicSelect from '../BasicSelect.vue';
import BasicSwitch from '../BasicSwitch.vue';

const props = defineProps<{
    fields: Record<string, string>
}>();

const emit = defineEmits<{
    (e: "update:fields", fields: Record<string, string>): void
}>();

const fields = ref<Record<string, string>>({});

watch(() => props.fields, (_fields) => {
    _fields.name = _fields["name"] ?? "\"\"";
    _fields.purchase_sequence = _fields["purchase_sequence"] ?? "{}";
    _fields.number = _fields["number"] ?? "Weapon.PRIMARY";
    _fields.switch_delay = _fields["switch_delay"] ?? "100";
    _fields.attack_button = _fields["attack_button"] ?? "Mouse.LEFT";
    _fields.horizontal_strafe_mode = _fields["horizontal_strafe_mode"] ?? "\"random\"";
    _fields.vertical_strafe_mode = _fields["vertical_strafe_mode"] ?? "\"oscillating\"";
    fields.value = _fields;
}, {
    immediate: true,
    deep: true
});

function update_field(key: string, value: string) {
    fields.value[key] = value;
    emit("update:fields", fields.value);
}

const weapon_number_options = [
    { description: "主武器", content: "Weapon.PRIMARY" },
    { description: "副武器", content: "Weapon.SECONDARY" },
    { description: "近战武器", content: "Weapon.MELEE" },
    { description: "手雷", content: "Weapon.GRENADE" }
];

const weapon_attack_buttons = [
    { description: "左键", content: "Mouse.LEFT" },
    { description: "右键", content: "Mouse.RIGHT" }
];

const check = (s: string) => {
    s = s.trim();
    if (!isNaN(+(s.trim()))) { return true; }
    if ([
        "Delay.MINI_MINI",
        "Delay.MINI",
        "Delay.SHORT",
        "Delay.NORMAL",
        "Delay.MEDIUM",
        "Delay.LONG",
        "Delay.LONG_LONG",
        "Delay.REFRESH"
    ].includes(s.replace(/\s+/g, ''))) {
        return true;
    }
    return false;
};

const VERTICAL_STRAFE_MODES = [
    { description: "无", content: "\"none\"" },
    { description: "固定向上", content: "\"up\"" },
    { description: "固定向下", content: "\"down\"" },
    { description: "随机", content: "\"random\"" },
    { description: "简谐振动（上下交替）", content: "\"oscillating\"" }
];
const HORIZONTAL_STRAFE_MODES = [
    { description: "无", content: "\"none\"" },
    { description: "固定向左", content: "\"left\"" },
    { description: "固定向右", content: "\"right\"" },
    { description: "随机", content: "\"random\"" },
    { description: "简谐振动（左右交替）", content: "\"oscillating\"" }
];
</script>

<template>
    <BasicField label="武器/装备名称" :quoted="true" :value="fields['name']" @update:value="update_field('name', $event)" />
    <br>
    <BasicKeystrokes label="购买按键序列" :value="fields['purchase_sequence']" @update:value="update_field('purchase_sequence', $event)"  />
    <br>
    <BasicSelect label="武器栏位" :value="fields['number']" @update:value="update_field('number', $event ?? 'Weapon.PRIMARY')" :options="weapon_number_options" />
    <br>
    <BasicField label="切换延迟" :value="fields['switch_delay']" @update:value="update_field('switch_delay', $event)" :check="check" />
    <br>
    <BasicSwitch label="攻击按键" :value="fields['attack_button']" @update:value="update_field('attack_button', $event)" :options="weapon_attack_buttons" />
    <br>
    <BasicSelect label="水平扫射方向" :value='fields["horizontal_strafe_mode"]' :options="HORIZONTAL_STRAFE_MODES" @update:value="update_field('horizontal_strafe_mode', $event ?? HORIZONTAL_STRAFE_MODES[0].content)" />
    <br>
    <BasicSelect label="垂直扫射方向" :value='fields["vertical_strafe_mode"]' :options="VERTICAL_STRAFE_MODES" @update:value="update_field('vertical_strafe_mode', $event ?? VERTICAL_STRAFE_MODES[0].content)" />
    <br>
    <div style="max-width: 50%;">
        <CodeSnippet format :snippet="GenerateWeaponCode(fields)" />
    </div>
</template>

<style lang="scss" scoped>
.code-block {
    max-height: 20em;
}
</style>
