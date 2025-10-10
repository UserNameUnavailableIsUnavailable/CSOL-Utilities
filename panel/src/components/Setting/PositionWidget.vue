<script lang="ts" setup>
import { computed, inject, reactive, ref, watch, type Ref } from 'vue';
import type { PositionWidget_T } from '../../scripts/Widget';
import BaseWidget from './BaseWidget.vue';
import CodeSnippet from '../CodeSnippet.vue';
import BasicField from '../BasicField.vue';

const props = defineProps<{
    widget: PositionWidget_T
}>();

props.widget.x.value = props.widget.x.value ?? "";
props.widget.y.value = props.widget.y.value ?? "";

const widget = reactive(props.widget);

const id_x = `POSITION_${widget.id}_X`;
const id_y = `POSITION_${widget.id}_Y`;
const SETTING_ITEMS = inject("SETTING_ITEMS") as Map<string, Ref<string>>;

const setting_item_x = ref(widget.x.value);
const setting_item_y = ref(widget.x.value);

if (!widget.ignore) {
    SETTING_ITEMS.set(id_x, setting_item_x as Ref<string>);
    SETTING_ITEMS.set(id_y, setting_item_y as Ref<string>);
}

const snippet = computed(() => {
    return `${id_x} = ${setting_item_x.value ? setting_item_x.value : "nil"}\n` +
        `${id_y} = ${setting_item_y.value ? setting_item_y.value : "nil"}\n`;
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
const SETTING_ITEM_STATES = inject("SETTING_ITEM_STATES") as Map<string, Ref<boolean>>;
SETTING_ITEM_STATES.set(id_x, enabled);
SETTING_ITEM_STATES.set(id_y, enabled);

const raw_position = ref("");

// 用户手动输入位置坐标
const position = computed({
    get() {
        return raw_position.value;
    },
    set(value: string) {
        raw_position.value = value;
        if (!value || value.trim().length === 0) {
            setting_item_x.value = "";
            setting_item_y.value = "";
            return;
        }
        const pattern = /^\s*\(\s*(\S+)\s*,\s*(\S+)\s*\)\s*$/;
        const match = value.match(pattern);
        if (match) {
            setting_item_x.value = match[1];
            setting_item_y.value = match[2];
        }
    }
});

// 位置坐标数据从外部导入，更新 raw_position
watch(() => [setting_item_x.value, setting_item_y.value], () => {
    if (setting_item_x.value?.trim().length === 0 && setting_item_y.value?.trim().length === 0) {
        raw_position.value = "";
    } else {
        raw_position.value = `(${setting_item_x.value}, ${setting_item_y.value})`;
    }
}, { immediate: true });

</script>

<template>
    <div v-show="enabled">
        <BaseWidget :widget="widget">
            <BasicField :id="'POSITION_' + widget.id + '_X'" :label="widget.label" v-model:value="position" />
            <input type="text" id="'POSITION_' + widget.id + '_X'" style="display: none;" v-model="setting_item_x" />
            <input type="text" id="'POSITION_' + widget.id + '_Y'" style="display: none;" v-model="setting_item_y" />
        </BaseWidget>
        <CodeSnippet :snippet="snippet" />
    </div>
</template>
