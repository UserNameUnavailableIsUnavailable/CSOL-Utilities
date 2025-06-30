<!-- Armor 和 PartWeapon 使用的配置控件 -->
<script lang="ts" setup>
import { GenerateWeaponCode } from '../../scripts/Weapon';
import CodeSnippet from '../CodeSnippet.vue';
import { computed, ref, watch } from 'vue';
import BasicKeystrokes from '../BasicKeystrokes.vue';
import BasicField from '../BasicField.vue';
import BasicSelect from '../BasicSelect.vue';
import YAML from "js-yaml";
import luaparse from "luaparse";
import { ResolveWeapon } from '../../scripts/Weapon';

const props = defineProps<{
    fields: Record<string, string>
    type: "Conventional" | "Special"
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

const options = ref<{ description: string, content: string, remark: string }[]>([]);

// 类型发生更新
watch(() => props.type, () => {
    options.value = [{
        description: '无',
        content: "nil",
        remark: "nil"
    }];
    fetch(`${TEMPLATE_BASE_URL}/listing.yaml`)
        .then(response => response.text())
        .then(text => {
            const listing = YAML.load(text) as {
                Conventional: { weapon: string, name: string, file: string }[]
                Special: { weapon: string, name: string, file: string }[]
            };
            listing[props.type].forEach(e => {
                options.value.push({ description: e.name, content: e.weapon, remark: e.file });
            });
        });
}, {
    immediate: true
});

const fields = ref<Record<string, string>>({});

watch(() => props.fields, (_fields) => {
    _fields.name = _fields["name"] ?? "\"\"";
    _fields.purchase_sequence = _fields["purchase_sequence"] ?? "{}";
    fields.value = _fields;
}, {
    immediate: true,
    deep: true
});

function update_field(key: string, value: string) {
    fields.value[key] = value;
    emit("update:fields", fields.value);
}

// 将模板合并到武器中
async function update_template(weapon: string) {
    if (weapon === "nil") {
        fields.value["template_name"] = "nil";
        return;
    }
    const item = options.value.find(e => e.content === weapon);
    if (!item) {
        alert("找不到指定的武器模板。");
        fields.value["template_name"] = "nil";
    } else { // 下载、解析武器模板，将武器模板字段合并到当前武器字段中
        await fetch(TEMPLATE_BASE_URL + `/${item.remark}`)
            .then(response => response.text())
            .then(code => {
                // @ts-ignore
                const ast = luaparse.parse(code).body[0].expression;
                const template_weapon: Record<string, string> = {};
                ResolveWeapon(ast, template_weapon);
                fields.value["template_name"] = `"${item.description}"`;
                for (const k in template_weapon) {
                    if (
                        // 排除用户自行定义的字段
                        k !== "name" &&
                        k !== "purchase_sequence" &&
                        k !== "template_name"
                    ) {
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
        emit("update:fields", fields.value)
    }
})
</script>

<template>
    <BasicField label="武器/装备名称" quoted :value="fields['name']"
        @update:value="update_field('name', $event)" />
    <br>
    <BasicKeystrokes label="购买按键序列" :value="fields['purchase_sequence']"
        @update:value="update_field('purchase_sequence', $event)" />
    <br>
    <BasicSelect label="模板" v-model:value="raw_template_name" :options="options" />
    <div style="max-width: 50%;">
        <CodeSnippet format :snippet="GenerateWeaponCode(fields)" />
    </div>
</template>

<style lang="scss" scoped>
.code-block {
    max-height: 20em;
}
</style>
