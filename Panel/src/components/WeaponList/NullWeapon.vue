<!-- Armor 和 PartWeapon 使用的配置控件 -->
<script lang="ts" setup>
import { ref, watch } from 'vue';
import BasicKeystrokes from '../BasicKeystrokes.vue';
import BasicField from '../BasicField.vue';
import CodeSnippet from '../CodeSnippet.vue';
import { GenerateWeaponCode } from '../../scripts/Weapon';

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

    fields.value = _fields;
}, {
    immediate: true,
    deep: true
});

function update_field(key: string, value: string) {
    fields.value[key] = value;
    emit("update:fields", fields.value)
}
</script>

<template>
    <BasicField label="武器/装备名称" quoted :value="fields.name" @update:value="update_field('name', $event)" />
    <br>
    <BasicKeystrokes label="购买按键序列" :value="fields.purchase_sequence" @update:value="update_field('purchase_sequence', $event)" />
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
