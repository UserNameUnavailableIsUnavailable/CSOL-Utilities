<script lang="ts" setup>
import { computed, onMounted, ref } from 'vue';
import { ElInput } from 'element-plus';

const props = defineProps<{
    id?: string // 字段 ID
    label?: string // 标签
    value?: string // 绑定的值
    quoted?: boolean // 是否需要
    hint?: string // 可选，用于提示
    check?: (s: string) => boolean // 字段校验函数
}>();

// 当用户输入的值发生更新时，通过 emit 告知上层
const emit = defineEmits<{
    (event_name: "update:value", value: string): void
}>();

// 字段是否合法
const legal = ref(true);

// 读写用户原始输入值
const raw_value = computed({
    get() {
        let ret = props.value;
        if (!ret) { // 处理空值
            return "";
        }
        if (props.check && !props.check(ret)) {
            legal.value = false; // 设置为非法
        }
        if (props.quoted) { // 移除引号得到原始值
            if ( // 如果被引号包裹
                ret.startsWith('\'') && ret.endsWith('\'') ||
                ret.startsWith('\"') && ret.endsWith('\"')
            ) {
                ret = ret.slice(1, -1); // 移除首尾的引号
            }
        }
        return ret.replace(/\\\\/g, "\\").replace(/\\\"/g, "\"").replace(/\\\'/g, "\'"); // 解析转义字符序列 \\ \" \'
    },
    set(v: string) { // 设置原始值
        const ret = props.quoted ? `"${v}"` : v; // 按需添加引号
        if (props.check && !props.check(ret)) { // 设置了校验函数，且校验未通过
            legal.value = false; // 字段值非法，不触发更新
        } else {
            legal.value = true;
        }
        emit("update:value", ret); // 字段值合法，触发更新
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
        <el-input size="small" style="width: 20em; display: inline-block; margin: 0.5em;" v-model="raw_value" autocomplete="off" />
        <!-- 字段值非法显示错误标记 -->
        <span v-if="!legal">❌️</span>
        &nbsp;
        <span v-if="props.hint" v-html="props.hint"></span>
    </div>
</template>
