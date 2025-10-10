<!-- Armor 和 PartWeapon 使用的配置控件 -->
<script lang="ts" setup>
import { GenerateWeaponCode, HORIZONTAL_STRAFE_MODES, VERTICAL_STRAFE_MODES } from '../../scripts/Weapon';
import CodeSnippet from '../CodeSnippet.vue';
import { computed, ref, watch } from 'vue';
import BasicKeystrokes from '../BasicKeystrokes.vue';
import BasicField from '../BasicField.vue';
import BasicSelect from '../BasicSelect.vue';
import YAML from "js-yaml";
import luaparse from "luaparse";
import { ResolveWeapon } from '../../scripts/Weapon';
import type { Language } from '../../scripts/Language';

const props = defineProps<{
    fields: Record<string, string>
    type: "Conventional" | "Special"
    remarks?: string[]
    language: Language
}>();

const emit = defineEmits<{
    (e: "update:fields", fields: Record<string, string>): void
}>();

const TEMPLATE_BASE_URL = "./weapon_templates";

function remove_surrounding_quotes(s: string) {
    if (
        s.startsWith('\'') && s.endsWith('\'') ||
        s.startsWith('\"') && s.endsWith('\"')
    ) {
        return s.slice(1, -1);
    }
    return s;
}

const options = ref<{ text: string, value: string, file: string }[]>([]);

type WeaponTemplateInfo_T = {
    id: string,
    en_US: string,
    zh_CN: string,
    file: string
}

// 类型发生更新
options.value = [{
    text: '⸺',
    value: "nil",
    file: "nil"
}];

// 下载武器模板列表
let listing: {
    Conventional: WeaponTemplateInfo_T[],
    Special: WeaponTemplateInfo_T[]
} | null = null;

fetch(`${TEMPLATE_BASE_URL}/listing.yaml`)
    .then(response => response.text())
    .then(text => {
        listing = YAML.load(text) as {
            Conventional: WeaponTemplateInfo_T[],
            Special: WeaponTemplateInfo_T[]
        };
        listing[props.type].forEach(e => {
            options.value.push({ text: e[props.language], value: e.id, file: e.file });
        });
        update_template(remove_surrounding_quotes(props.fields["template_name"] ?? "nil"));
        emit("update:fields", fields.value);
    });

const fields = ref<Record<string, string>>({});

const user_defined_fields = [
    "name", "purchase_sequence", "template_name", "attack_duration",
    "horizontal_strafe_mode", "vertical_strafe_mode"
];

watch(() => props.fields, (_fields) => {
    _fields.name = _fields["name"] ?? "\"\"";
    _fields.purchase_sequence = _fields["purchase_sequence"] ?? "{}";
    _fields.horizontal_strafe_mode = _fields["horizontal_strafe_mode"] ?? "\"random\"";
    _fields.vertical_strafe_mode = _fields["vertical_strafe_mode"] ?? "\"none\"";
    _fields.attack_duration = _fields["attack_duration"] ?? "10";
    _fields.template_name = _fields["template_name"] ?? "nil";
    fields.value = _fields;
}, {
    immediate: true,
    deep: true
});

watch(() => props.language, () => {
    options.value.length = 1;
    listing?.[props.type].forEach(e => {
        options.value.push({ text: e[props.language], value: e.id, file: e.file });
    });
});

function update_field(key: string, value: string) {
    fields.value[key] = value;
    emit("update:fields", fields.value);
}

// 将模板合并到武器中
async function update_template(template_name: string) {
    if (template_name === "nil") {
        fields.value["template_name"] = "nil";
        return;
    }
    const item = options.value.find(e => e.value === template_name);
    if (!item) {
        alert(`找不到指定的武器模板：${template_name}`);
        fields.value["template_name"] = "nil";
    } else { // 下载、解析武器模板，将武器模板字段合并到当前武器字段中
        await fetch(TEMPLATE_BASE_URL + `/${item.file}`)
            .then(response => response.text())
            .then(code => {
                // @ts-ignore
                const ast = luaparse.parse(code).body[0].expression;
                const template_weapon: Record<string, string> = {};
                ResolveWeapon(ast, template_weapon);
                fields.value["template_name"] = `"${item.value}"`;
                for (const k in fields.value) {
                    if (!user_defined_fields.includes(k)) { // 非用户定义字段，删除
                        delete fields.value[k];
                    }
                }
                for (const k in template_weapon) {
                    if (!user_defined_fields.includes(k)) { // 非用户定义字段，使用模板字段
                        fields.value[k] = template_weapon[k];
                    }
                }
            });
    }
}

// 不加引号的武器名
const raw_template_name = computed({
    get() {
        return remove_surrounding_quotes(props.fields["template_name"] ?? "nil");
    },
    async set(v: string) {
        await update_template(v);
        emit("update:fields", fields.value);
        const name = options.value.find(e => e.value === v)?.text ?? "";
        fields.value["name"] = `"${name}"`;
    }
});

</script>

<template>
    <BasicField label="武器/装备名称" quoted :value="fields['name']"
        @update:value="update_field('name', $event)" />
    <BasicKeystrokes label="购买按键序列" :value="fields['purchase_sequence']"
        @update:value="update_field('purchase_sequence', $event)" />
    <BasicSelect label="模板" v-model:value="raw_template_name" :options="options" />
    <BasicField label="每轮攻击持续时间（秒）" :value="fields['attack_duration']"
        @update:value="update_field('attack_duration', $event)" />
    <BasicSelect label="水平扫射方向" :value='fields["horizontal_strafe_mode"]' :options="HORIZONTAL_STRAFE_MODES" @update:value="update_field('horizontal_strafe_mode', $event ?? HORIZONTAL_STRAFE_MODES[0].value)" />
    <BasicSelect label="垂直扫射方向" :value='fields["vertical_strafe_mode"]' :options="VERTICAL_STRAFE_MODES" @update:value="update_field('vertical_strafe_mode', $event ?? VERTICAL_STRAFE_MODES[0].value)" />
    <div>
        <ul>
            <li v-for="remark in remarks" :key="remark" v-html="remark"></li>
        </ul>
    </div>

    <div>
        <CodeSnippet format :snippet="GenerateWeaponCode(fields)" />
    </div>
</template>

<style lang="scss" scoped>
.code-block {
    max-height: 20em;
}
</style>
