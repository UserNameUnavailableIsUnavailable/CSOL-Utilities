<template>
    <BasicField v-model:value="value" quoted @update:legal="legal=$event" :check="check" />
    <p>{{ value }}</p>
    <p v-if="!legal" style="color: red;">字段值不合法</p>
</template>

<script lang="ts" setup>
import BasicField from './components/BasicField.vue';
import { ref } from 'vue';

function check(v: string): boolean {
    return v.length <= 5;
}

const value = ref('');
const legal = ref(true);
</script>