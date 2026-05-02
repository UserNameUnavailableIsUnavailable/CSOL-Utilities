<script lang="ts" setup>
import { type FieldWidget_T } from '../../scripts/Widget';
import { computed, inject, ref, type Ref } from 'vue';
import BaseWidget from './BaseWidget.vue';
import BasicField from '../BasicField.vue';
import CodeSnippet from '../CodeSnippet.vue';

const prop = defineProps<{
    widget: FieldWidget_T
}>();
const emit = defineEmits<{
    (e: 'update:legal', legal: boolean): void
}>();

const widget = prop.widget;

const id = `FIELD_${widget.id}`; // 字段 id

const SETTING_ITEMS = inject("SETTING_ITEMS") as Map<string, Ref<string>>; // 所有字段
const SETTING_ITEM_STATES = inject("SETTING_ITEM_STATES") as Map<string, Ref<boolean>>; // 字段状态（是否启用）
const SETTING_SWITCHES = inject("SETTING_SWITCHES") as Map<string, Ref<string>>; // 所有开关依赖

const field = ref(widget.value ?? "");

SETTING_ITEMS.set(id, field);

const snippet = computed(() => {
    return `${id} = ${field.value ? field.value : "nil"}`;
});

const enabled = computed(() => {
    if (!widget.depends_on) {
        return true;
    }
    return widget.depends_on.every(dep => {
        return dep.value === SETTING_SWITCHES.get(dep.key)?.value;
    });
});
// 这里要求依赖项在创建此组件之前就已经定义
SETTING_ITEM_STATES.set(id, enabled);
</script>

<template>
    <div v-show="enabled">
        <BaseWidget :widget="widget" >
            <BasicField :id="'FIELD_' + widget.id" :label="widget.label" v-model:value="field" :quoted="widget.quoted" :hint="widget.hint" @update:legal="emit('update:legal', $event)" />
        </BaseWidget>
        <CodeSnippet :snippet="snippet" />
    </div>
</template>
