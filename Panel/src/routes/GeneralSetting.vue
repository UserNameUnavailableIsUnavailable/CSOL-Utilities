<template>
    <div>
    </div>
</template>

<script setup lang="ts">
import { invoke } from "@tauri-apps/api/core"
import { onMounted, ref } from "vue"

const src = ref<string>("");

async function generateExecutorSrc() {
    const res = await invoke<string>("generate_executor_src", { path: "C:\\Executor" });
    return res;
}

onMounted(async () => {
    src.value = await generateExecutorSrc();
});
</script>
