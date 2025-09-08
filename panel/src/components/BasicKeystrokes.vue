<script lang="ts" setup>
import { computed, onMounted, ref, watch } from 'vue';
import { KEYMAP, REVERSE_KEYMAP } from '../scripts/KeyMap';

const props = defineProps<{
    id?: string // 字段 ID
    label?: string
    value?: string
    hint?: string
}>();

const emit = defineEmits<{
    (e: "update:value", value: string): void
}>();

// 按键列表，形如 ["Keyboard.A", "Keyboard.B"]
const keystroke_list = ref<string[]>([]);

// 输入框中对用户展示的结果
const display = computed(() =>
    keystroke_list.value.map(k => REVERSE_KEYMAP.get(k)).join(' ')
);

// 传入的参数发生更新
watch(() => props.value, (new_keystrokes) => {
    if (new_keystrokes) {
        const compact = new_keystrokes.replace(/\s/g, ""); // 去除所有空白
        let comma_splitted_tokens = compact.replace(/^\{(.*)\}$/g, "$1"); // 去掉大括号
        if (comma_splitted_tokens.endsWith(',')) {
            comma_splitted_tokens = comma_splitted_tokens.slice(0, -1); // 忽略末尾逗号
        }
        if (comma_splitted_tokens === "") {
            keystroke_list.value = [];
        } else {
            keystroke_list.value = comma_splitted_tokens.split(',');
        }
    } else { // 空值
        keystroke_list.value = [];
        emit("update:value", `{}`); // 触发上层更新，修改为非空值
    }
}, {
    immediate: true
});

// 每次输入后都需要通过 emit 告知上级更新 keystrokes
function on_keyup(e: KeyboardEvent) {
    const k = e.key.toUpperCase();
    if (KEYMAP.has(k)) {
        const key = KEYMAP.get(k) as string;
        keystroke_list.value.push(key);
        emit("update:value", `{${keystroke_list.value.join(", ")}}`);
    } else if (k === 'BACKSPACE') {
        let l = keystroke_list.value.length;
        l = l > 0 ? l - 1 : 0;
        keystroke_list.value.length = l;
        emit("update:value", `{${keystroke_list.value.join(", ")}}`);
    }
}

function on_focus() {
    document.addEventListener("keyup", on_keyup);
}

function on_blur() {
    document.removeEventListener("keyup", on_keyup);
}

const root = ref<HTMLInputElement>();
onMounted(() => {
    if (props.id && root.value) {
        root.value.id = props.id;
    }
})
</script>

<template>
    <label ref="root">
        <span v-if="label" v-html="label"></span>
        &nbsp;
        <input type="text" readonly @focus="on_focus" @blur="on_blur" :value="display" autocomplete="off" />
    </label>
</template>
