<script lang="ts" setup>
import { computed } from 'vue';

const props = defineProps<{
    label?: string // 标签
    value?: string // 绑定的值
    options: { description: string, content: string }[] // 选项
}>();

const emit = defineEmits<{
    (event_name: "update:value", value: string): void
}>();

const value = computed({
    get() { return props.value; },
    set(v: string) { emit("update:value", v); }
})
</script>

<template>
    <label>
        <span v-if="props.label" v-html="props.label"></span>
        &nbsp;
        <template v-for="opt in props.options">
            <input type="radio" :checked="value === opt.content" @change="value = opt.content" autocomplete="off" >
            <label v-html="opt.description"></label>
            &nbsp;
        </template>
    </label>
</template>
