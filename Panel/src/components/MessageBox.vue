<template>
  <el-dialog
    v-model="dialogVisible"
    :title="props.title"
    width="85%"
    :close="handleClose"
  >
    <span>{{ props.text }}</span>
    <template #footer>
      <div class="dialog-footer">
        <el-button type="primary" @click="dialogVisible = false; okCallback?.();">
          {{ props.okText }}
        </el-button>
        <el-button @click="dialogVisible = false; cancelCallback?.();">{{ props.cancelText }}</el-button>
      </div>
    </template>
  </el-dialog>
</template>

<script lang="ts" setup>
import { computed } from 'vue';

const props = defineProps<{
    visible: boolean,
    title: string,
    text: string,
    okText?: string,
    cancelText?: string,
    okCallback?: () => void,
    cancelCallback?: () => void
}>();

const emit = defineEmits<{
    (e: 'update:visible', value: boolean): void
}>();

const dialogVisible = computed({
    get() {
        return props.visible;
    },
    set(v : boolean) {
        emit('update:visible', v);
    }
});

const handleClose = () => {
    dialogVisible.value = false;
    props.cancelCallback?.();
};
</script>
