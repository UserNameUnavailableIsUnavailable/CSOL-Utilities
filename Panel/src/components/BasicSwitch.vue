<script lang="ts" setup>
import { computed, onMounted, ref } from 'vue';
import { ElRadioGroup, ElRadioButton } from 'element-plus';

const props = defineProps<{
    id?: string // 字段 ID
    label?: string // 标签
    value?: string // 绑定的值
    options: { text: string, value: string }[] // 选项
}>();

const emit = defineEmits<{
    (event_name: "update:value", value: string): void
}>();

const value = computed({
    get() { return props.value; },
    set(v: string) { emit("update:value", v); }
})

const root = ref<HTMLInputElement>();

onMounted(() => {
    if (props.id && root.value) {
        root.value.id = props.id;
    }
});
</script>

<template>
    <div ref="root">
        <span v-if="props.label" v-html="props.label"></span>
            &nbsp;
            <el-radio-group fill="magenta" text-color="white" size="small" v-model="value" style="display: inline-block; margin: 0.5em;">
                <el-radio-button v-for="opt in props.options" :label="opt.text" :key="opt.value" :value="opt.value" />
            </el-radio-group>
    </div>
</template>
