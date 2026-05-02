<script lang="ts" setup>
import ExecutorSetting from './routes/ExecutorSetting.vue';
import ExecutorWeaponList from './routes/ExecutorWeaponList.vue';
import ControllerSetting from './routes/ControllerSetting.vue';
import { RouterView } from 'vue-router';
import router from './router';
import { onMounted, ref } from 'vue';
import Introduction from './routes/Introduction.vue';

const routes = [
    {
        path: "/",
        component: Introduction
    },
    {
        path: "/ExecutorSetting",
        component: ExecutorSetting
    },
    {
        path: "/ExecutorWeaponList",
        component: ExecutorWeaponList
    },
    {
        path: "/ControllerSetting",
        component: ControllerSetting
    }
];

const intro = ref();
let active_button: HTMLButtonElement | undefined;
onMounted(() => {
    routes.forEach(route => {
        router.addRoute(route);
    });
    router.push("/");
    intro.value.click();
});
const onClick = (event: MouseEvent, route: string) => {
    const e = event.target as HTMLButtonElement;
    if (active_button) {
        active_button.classList.remove("active");
        active_button.classList.add("inactive");
    }
    active_button = e;
    e.classList.add("active");
    router.push(route);
}

</script>

<template>
    <div class="container">
        <div class="navigation">
            <button ref="intro" class="active tab" @click="onClick($event, '/')">简介</button>
            <button class="inactive tab" @click="onClick($event, '/ExecutorSetting')">执行器：通用设定</button>
            <button class="inactive tab" @click="onClick($event, '/ExecutorWeaponList')">执行器：挂机武器列表</button>
            <button class="inactive tab" @click="onClick($event, '/ControllerSetting')">控制器：通用设定</button>
        </div>
        <div class="panel">
            <RouterView v-slot="{ Component }">
                <KeepAlive>
                    <component :is="Component" />
                </KeepAlive>
            </RouterView>
        </div>
    </div>
</template>

<style>
.navigation {
    width: 15%;
    position: fixed;
    top: 0;
    left: 0;
    z-index: 10;
    display: flex;
    flex-direction: column;
    align-items: stretch;
    padding-top: 0.5em;
}

.panel {
    width: 84%;
    position: absolute;
    top: 0;
    right: 0;
}

.inactive.tab {
	background-color: white;
    color: steelblue;
}

.active.tab {
	background-color: steelblue;
    color: white;
}

.tab {
    width: auto;
	border-radius: 10px 0px 0px 10px;
	padding: 1em 0.25em;
	text-decoration: none;
}
</style>

