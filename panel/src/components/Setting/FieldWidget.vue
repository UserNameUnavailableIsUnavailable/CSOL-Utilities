<script lang="ts" setup>
import { type FieldWidget_T } from '../../scripts/Widget';
import { computed, inject, ref, type Ref } from 'vue';
import BaseWidget from './BaseWidget.vue';
import BasicField from '../BasicField.vue';
import CodeSnippet from '../CodeSnippet.vue';
const props = defineProps<{
    widget: FieldWidget_T
}>();
const widget = props.widget; // widget 来自固定的配置文件，不需要响应式

const id = `FIELD_${widget.id}`; // 字段 id

const SETTING_ITEM_STATES = inject("SETTING_ITEM_STATES") as Map<string, Ref<boolean>>;
const SETTING_ITEMS = inject("SETTING_ITEMS") as Map<string, Ref<string>>;
const SETTING_SWITCHES = inject("SETTING_SWITCHES") as Map<string, Ref<string>>;

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
            <BasicField :label="widget.label" v-model:value="field" :quoted="widget.quoted" :hint="widget.hint" />
        </BaseWidget>
        <CodeSnippet :snippet="snippet" />
    </div>
</template>
