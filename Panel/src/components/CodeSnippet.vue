<script lang="ts" setup>
import { nextTick, watch } from 'vue';
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
</script>

<template>
    <pre class="code-block"><code class="language-lua code-block">{{ props.format ? formatText(snippet) : snippet }}</code></pre>
</template>
