<script lang="ts" setup>
import { type SelectWidget_T } from "../../scripts/Widget";
import { computed, inject, reactive, ref, type Ref } from 'vue';
import BaseWidget from "./BaseWidget.vue";
import BasicSelect from "../BasicSelect.vue";
import CodeSnippet from "../CodeSnippet.vue";

const props = defineProps<{
    widget: SelectWidget_T
}>();

const widget = reactive(props.widget);

const setting_item = ref(widget.value ?? widget.options[0].value);  // 缺省时选中第一个选项

const id = `SELECT_${widget.id}`;

const SETTING_ITEMS = inject("SETTING_ITEMS") as Map<string, Ref<any>>;

if (!widget.ignore) {
    SETTING_ITEMS.set(id, setting_item);
}

const snippet = computed(() => {
    return `SELECT_${widget.id} = ${setting_item.value}`;
}); // Lua 代码片段

const SETTING_SWITCHES = inject("SETTING_SWITCHES") as Map<string, Ref<string>>;
const enabled = computed(() => {
    if (!widget.depends_on) {
        return true;
    }
    return widget.depends_on.every(dep => {
        return dep.value === SETTING_SWITCHES.get(dep.key)?.value;
    });
});

</script>

<template>
    <div v-if="widget" v-show="enabled">
        <BaseWidget :widget="widget" />
        <BasicSelect :id="id" :label="widget.label" :options="widget.options" v-model:value="setting_item" />
        <CodeSnippet :snippet="snippet" />
    </div>
</template>
