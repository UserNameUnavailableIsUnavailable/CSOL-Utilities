<script lang="ts" setup>
import { computed, onMounted, ref } from 'vue';
import { ElInput } from 'element-plus';

const props = defineProps<{
    id?: string // 字段 ID
    label?: string // 标签
    value?: string // 绑定的值
    quoted?: boolean // 是否需要引号
    hint?: string // 可选，用于提示
    check?: (s: string) => boolean // 字段校验函数
}>();

// props.value 更新 -> raw_value 更新
// raw_value 更新，通过 emit 通知上层更新 props.value

// 当用户输入的值发生更新时，通过 emit 告知上层
const emit = defineEmits<{
    (event_name: "update:value", value: string): void // 字段值发生更新
    (event_name: "update:legal", legal: boolean): void // 字段是否合法发生更新
}>();

const legal = ref(true);

const raw = computed({
    get() {
        let v = props.value;
        if (!v) {
            v = props.quoted ? '""' : "";
            emit("update:value", v); // 通知上层更正为空串
            return "";
        }
        if (props.check && !props.check(v)) {
            legal.value = false;
            emit("update:legal", false);
        }
        if (props.quoted) {
            if (
                v.startsWith('\'') && v.endsWith('\'') ||
                v.startsWith('\"') && v.endsWith('\"')
            ) {
                v = v.slice(1, -1);
                return v.replace(/\\\\/g, "\\").replace(/\\\"/g, "\"").replace(/\\\'/g, "\'");
            }
        }
        return v;
    },
    set(v: string) {
        if (props.quoted) {
            v = v.replace(/\\/g, "\\\\").replace(/\"/g, "\\\"").replace(/\\\'/g, "\\\'");
            v = `"${v}"`;
        }
        if (props.check && !props.check(v)) {
            legal.value = false;
            emit("update:legal", false);
        } else {
            legal.value = true;
            emit("update:legal", true);
        }
        emit("update:value", v);
    }
});

const root = ref<HTMLInputElement>();
onMounted(() => {
    if (props.id && root.value) {
        root.value.id = props.id;
    }
})
</script>

<template>
    <div ref="root">
        <span v-if="props.label" v-html="props.label"></span>
        &nbsp;
        <el-input size="small" style="width: 20em; display: inline-block; margin: 0.5em;" v-model="raw" autocomplete="off" />
        <!-- 字段值非法显示错误标记 -->
        <span v-if="!legal">❌️</span>
        &nbsp;
        <span v-if="props.hint" v-html="props.hint"></span>
    </div>
</template>
