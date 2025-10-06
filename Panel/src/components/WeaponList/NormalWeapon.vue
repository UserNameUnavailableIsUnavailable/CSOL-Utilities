<!-- ConventionalWeapon 使用的配置控件 -->
<script lang="ts" setup>

import { ref, watch } from 'vue';
import CodeSnippet from '../CodeSnippet.vue';
import { GenerateWeaponCode, HORIZONTAL_STRAFE_MODES, VERTICAL_STRAFE_MODES } from '../../scripts/Weapon';
import BasicField from '../BasicField.vue';
import BasicKeystrokes from '../BasicKeystrokes.vue';
import BasicSelect from '../BasicSelect.vue';
import BasicSwitch from '../BasicSwitch.vue';

const props = defineProps<{
    fields: Record<string, string>
    remarks?: string[]
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
    _fields.vertical_strafe_mode = _fields["vertical_strafe_mode"] ?? "\"none\"";
    _fields.attack_duration = _fields["attack_duration"] ?? "10";
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
    { text: "主武器", value: "Weapon.PRIMARY" },
    { text: "副武器", value: "Weapon.SECONDARY" },
    { text: "近战武器", value: "Weapon.MELEE" },
    { text: "手雷", value: "Weapon.GRENADE" }
];

const weapon_attack_buttons = [
    { text: "左键", value: "Mouse.LEFT" },
    { text: "右键", value: "Mouse.RIGHT" }
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
</script>

<template>
    <BasicField label="武器/装备名称" :quoted="true" :value="fields['name']" @update:value="update_field('name', $event)" />
    <br>
    <BasicKeystrokes label="购买按键序列" :value="fields['purchase_sequence']" @update:value="update_field('purchase_sequence', $event)"  />
    <br>
    <BasicSelect label="武器栏位" :value="fields['number']" @update:value="update_field('number', $event ?? 'Weapon.PRIMARY')" :options="weapon_number_options" />
    <br>
    <BasicField label="切换延迟（毫秒）" :value="fields['switch_delay']" @update:value="update_field('switch_delay', $event)" :check="check" />
    <br>
    <BasicSwitch label="攻击按键" :value="fields['attack_button']" @update:value="update_field('attack_button', $event)" :options="weapon_attack_buttons" />
    <br>
    <BasicField label="每轮攻击持续时间（秒）" :value="fields['attack_duration']" @update:value="update_field('attack_duration', $event)" :check="check" />
    <br>
    <BasicSelect label="水平扫射方向" :value='fields["horizontal_strafe_mode"]' :options="HORIZONTAL_STRAFE_MODES" @update:value="update_field('horizontal_strafe_mode', $event ?? HORIZONTAL_STRAFE_MODES[0].value)" />
    <br>
    <BasicSelect label="垂直扫射方向" :value='fields["vertical_strafe_mode"]' :options="VERTICAL_STRAFE_MODES" @update:value="update_field('vertical_strafe_mode', $event ?? VERTICAL_STRAFE_MODES[0].value)" />
    <div>
        <ul>
            <li v-for="remark in remarks" :key="remark" v-html="remark"></li>
        </ul>
    </div>
    <div style="max-width: 50%;">
        <CodeSnippet format :snippet="GenerateWeaponCode(fields)" />
    </div>
</template>

<style lang="scss" scoped>
.code-block {
    max-height: 20em;
}
</style>
