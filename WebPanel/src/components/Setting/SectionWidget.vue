<script lang="ts" setup>
import { type FieldWidget_T, type KeystrokesWidget_T, type PositionWidget_T, type SectionWidget_T, type SwitchWidget_T, type SelectWidget_T } from "../../scripts/Widget";
import { computed, inject, reactive, type Ref } from 'vue';
import BaseWidget from "./BaseWidget.vue";
import SwitchWidget from "./SwitchWidget.vue";
import PositionWidget from "./PositionWidget.vue";
import FieldWidget from "./FieldWidget.vue";
import KeystrokesWidget from "./KeystrokesWidget.vue";
import SelectWidget from "./SelectWidget.vue";

const props = defineProps<{
    widget: SectionWidget_T
}>();

props.widget.level = props.widget.level ?? 1;
let next_level = (props.widget.level as number) + 1;
if (next_level > 6) { next_level = 6; }
props.widget.children = props.widget.children ?? [];
props.widget.children.forEach(child => {
    child.level = next_level as 1 | 2 | 3 | 4 | 5 | 6;
});
const widget = reactive(props.widget);
const SETTING_SWITCHES = inject("SETTING_SWITCHES")as Map<string, Ref<string>>;
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
        <template v-for="(child, index) in widget.children" :key="index">
            <SectionWidget :id="'SECTION' + widget.id" v-if="child.type === 'SECTION'" :widget="child as SectionWidget_T" />
            <FieldWidget v-if="child.type === 'FIELD'" :widget="child as FieldWidget_T" />
            <SwitchWidget v-else-if="child.type === 'SWITCH'" :widget="child as SwitchWidget_T" />
            <SelectWidget v-else-if="child.type === 'SELECT'" :widget="child as SelectWidget_T" />
            <PositionWidget v-else-if="child.type === 'POSITION'" :widget="child as PositionWidget_T" />
            <KeystrokesWidget v-else-if="child.type === 'KEYSTROKES'" :widget="child as KeystrokesWidget_T" />
        </template>
    </div>
</template>
