<script lang="ts" setup>
import { computed, nextTick, watch } from 'vue';
import CodeHighlighter from '../scripts/CodeHighlighter';
import { formatText } from 'lua-fmt';

const props = defineProps<{
    snippet: string; // 代码片段
    format?: boolean; // 是否需要进行代码格式化
}>();
const code_highlighter = new CodeHighlighter();

watch(() => props.snippet, async () => {
    await nextTick();
    code_highlighter.highlight_all();
}, {
    immediate: true
});

const content = computed(() => {
    if (props.format) {
        try {
            return formatText(props.snippet);
        } catch (e) {
            console.error("代码格式化失败：", e);
            return props.snippet;
        }
    } else {
        return props.snippet;
    }
});
</script>

<template>
    <pre class="code-block"><code class="language-lua code-block">{{ content }}</code></pre>
</template>
