import { createApp } from 'vue'
import App from './App.vue'
import router from "./router";

const app = createApp(App);
app.provide("VERSION", "\"1.5.2\"");
app.use(router);
app.mount('#app');
