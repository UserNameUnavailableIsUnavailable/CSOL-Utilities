<script lang="ts" setup>
    import { reactive, ref } from 'vue';
    import type { BaseWidget_T } from '../../scripts/Widget';
    const props = defineProps<{widget: BaseWidget_T}>();
    const base_widget = reactive(props.widget);
    const show_image = ref(false);
    const pop_up_image = ref<HTMLElement|null>(null);
    const on_hover = () => {
        show_image.value = true;
    }
    const on_leave = () => {
        show_image.value = false;
    }
</script>

<template>
    <component
        :is="`h${base_widget.level ?? 1}`"
        v-html="base_widget.title"
        @mouseover="on_hover"
        @mouseleave="on_leave"
        :style="{ color: base_widget.image ? 'blue' : 'black' }"
    ></component>
    <p v-if="base_widget.annotation" v-html="base_widget.annotation"></p>
    <img ref="pop_up_image" class="PopUpImage" v-if="base_widget.image && show_image" :src="base_widget.image" />
    <slot></slot>
</template>

<style>
.PopUpImage {
    position: fixed;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%); /* 初始位置在屏幕中心 */
    width: 50vw;    
    border: 1px solid #ccc;
    background-color: #fff;
    padding: 10px;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    z-index: 114514;
}
</style>
