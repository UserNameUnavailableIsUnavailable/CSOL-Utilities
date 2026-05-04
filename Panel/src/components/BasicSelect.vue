<script lang="ts" setup>
import { computed, onMounted, ref } from 'vue';
import { ElSelect, ElOption } from 'element-plus';

const props = defineProps<{
    id?: string // 字段 ID
    label?: string // 标签
    value?: string // 绑定的值
    options: { text: string, value: string }[] // 选项
}>();

const emit = defineEmits<{
    (event_name: "update:value", value: string | undefined): void
}>();

const value = computed({
    get() {
        return props.value;
    },
    set(v: string|undefined) {
        if (!v) {
            v = props.options[0]?.value;
        }
        emit("update:value", v);
    }
});

const root = ref<HTMLSelectElement | null>();

onMounted(() => {
    if (props.id && root.value) {
        root.value.id = props.id;
    }
});
</script>

<template>
    <div ref="root">
        <span v-if="props.label" v-html="props.label"></span>
        <el-select size="small" v-model="value" style="display: inline-block; width: 20em; margin: 0.5em;">
            <el-option v-for="opt in props.options" :key="opt.value" :value="opt.value" :label="opt.text" />
        </el-select>
    </div>
</template>
