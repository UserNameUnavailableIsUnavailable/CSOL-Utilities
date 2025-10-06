<script lang="ts" setup>
import { computed, inject, reactive, ref, type Ref } from 'vue';
import { type KeystrokesWidget_T } from '../../scripts/Widget';
import BasicKeystrokes from '../BasicKeystrokes.vue';
import BaseWidget from './BaseWidget.vue';
import CodeSnippet from '../CodeSnippet.vue';

const props = defineProps<{
    widget: KeystrokesWidget_T
}>();

const widget = reactive(props.widget);
const id = `KEYSTROKES_${widget.id}`;
const keystrokes = ref(widget.value ?? "{}");

const SETTING_ITEMS = inject("SETTING_ITEMS") as Map<string, Ref<string>>;

if (!widget.ignore) {
    SETTING_ITEMS.set(id, keystrokes);
}

const snippet = computed(() => {
    return `${id} = ${keystrokes.value}\n`;
});

const SETTING_SWITCHES = inject("SETTING_SWITCHES") as Map<string, Ref<string>>;
const enabled = computed(() => {
    if (!widget.depends_on) {
        return true;
    }
    return widget.depends_on.every(dep => {
        return dep.value === SETTING_SWITCHES.get(dep.key)?.value;
    });
});
// 这里要求依赖项在创建此组件之前就已经定义
const SETTING_ITEM_STATES = inject("SETTING_ITEM_STATES") as Map<string, Ref<boolean>>;
SETTING_ITEM_STATES.set(id, enabled);
</script>

<template>
    <div v-show="enabled">
        <BaseWidget :widget="widget" />
        &nbsp;
        <BasicKeystrokes :id="'KEYSTROKES_' + widget.id" :label="widget.label" v-model:value="keystrokes" />
        <CodeSnippet :snippet="snippet" />
    </div>
</template>
