import { createApp } from 'vue'
import App from './App.vue'
import router from "./router";

const app = createApp(App);
app.provide("VERSION", "1.5.3");
app.use(router);
app.mount('#app');
