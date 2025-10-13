<script lang="ts" setup>
import { computed, watch, onMounted, ref } from 'vue';
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


// const raw_value = computed({
//     // 显示在输入框中的值（没有引号）
//     get() {
//         let v = props.value;
//         if (v === undefined || v === null) { // 处理空值
//             emit("update:value", ""); // 更正为空串
//             emit("update:legal", true); // 更正为非法
//             return "";
//         }
//         // 设定了校验函数，且校验未通过
//         if (props.check && !props.check(v)) {
//             emit("update:legal", false);
//             return "";
//         }
//         if (props.quoted) { // 移除引号得到原始值
//             if ( // 如果被引号包裹
//                 v.startsWith('\'') && v.endsWith('\'') ||
//                 v.startsWith('\"') && v.endsWith('\"')
//             ) {
//                 v = v.slice(1, -1); // 移除首尾的引号
//                 // 被引号包裹的字符串中，转义字符序列 \\ \" \' 需要被解析
//                 return v.replace(/\\\\/g, "\\").replace(/\\\"/g, "\"").replace(/\\\'/g, "\'"); // 解析转义字符序列 \\ \" \'
//             }
//         }
//         return v;
//     },
//     set(v: string) { // 设置原始值
//         if (props.quoted) { // 如果需要引号，则对输入的值进行转义
//             v = v.replace(/\\/g, "\\\\").replace(/\"/g, "\\\"").replace(/\'/g, "\\\'"); // 转义字符序列 \ " '
//             v = `"${v}"`; // 添加引号
//         }
//         if (props.check && !props.check(v)) { // 设置了校验函数，且校验未通过
//             legal.value = false; // 字段值非法，不触发更新
//             emit("update:legal", false);
//         } else {
//             legal.value = true;
//             emit("update:legal", true);
//         }
//         emit("update:value", v); // 字段值合法，触发更新
//     }
// });

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
