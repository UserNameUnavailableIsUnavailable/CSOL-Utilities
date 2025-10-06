<script lang="ts" setup>
import GeneralSetting from './routes/GeneralSetting.vue';
import WeaponList from './routes/WeaponList.vue';
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
        path: "/GeneralSetting",
        component: GeneralSetting
    },
    {
        path: "/WeaponList",
        component: WeaponList
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
        <div class="tab-group">
            <button ref="intro" class="active tab" @click="onClick($event, '/')">简介</button>
            <button class="inactive tab" @click="onClick($event, '/GeneralSetting')">通用设定</button>
            <button class="inactive tab" @click="onClick($event, '/WeaponList')">武器列表</button>            
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
.container {
	display: grid;
	grid-template-columns: 10em auto;
	grid-template-rows: 800px 50px;
	grid-column-gap: 0px;
	grid-row-gap: 0px;
}

.tab-group {
    position: fixed;
    top: 0;
    left: 0;
    width: 10em;
    height: 100vh;
    z-index: 10;
    display: flex;
    flex-direction: column;
    align-items: stretch;
    padding-top: 0.5em;
}

.panel {
    grid-area: 1 / 2 / 2 / 3;
}

.inactive.tab {
	background-color: gray;
    color: white;
}

.active.tab {
	background-color: white;
    color: gray;
}

.tab {
    width: 10em;
	border-radius: 10px 0px 0px 10px;
	padding: 10px 20px 10px 20px;
	text-decoration: none;
}

</style>

