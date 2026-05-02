import { createApp } from 'vue';
import ElementPlus from 'element-plus';
import 'element-plus/dist/index.css';
import App from './App.vue'
import router from "./router";

const app = createApp(App);
app.provide("VERSION", "1.5.6");
app.provide("language", "zh_CN");
app.use(ElementPlus);
app.use(router);
app.mount('#app');
