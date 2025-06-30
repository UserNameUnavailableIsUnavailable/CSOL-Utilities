<script lang="ts" setup>
import { computed } from 'vue';

const props = defineProps<{
    label?: string // 标签
    value?: string // 绑定的值
    options: { description: string, content: string }[] // 选项
}>();

const emit = defineEmits<{
    (event_name: "update:value", value: string | undefined): void
}>();

const value = computed({
    get() {
        return props.value;
    },
    set(v: string) {
        emit("update:value", v);
    }
})
</script>

<template>
    <label>
        <span v-if="props.label" v-html="props.label"></span>
        &nbsp;
        <select v-model="value">
            <option v-for="opt in props.options" :value="opt.content">{{ opt.description }}</option>
        </select>
    </label>
</template>
