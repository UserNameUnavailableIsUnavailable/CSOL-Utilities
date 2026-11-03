<template>
	<div class="login-page">
		<el-card class="login-card">
			<template #header>
				<span>用户登录</span>
			</template>

			<el-form ref="formRef" :model="form" :rules="rules" label-position="top" @submit.prevent>
				<el-form-item label="用户名" prop="username">
					<el-input v-model="form.username" autocomplete="username" placeholder="请输入用户名" />
				</el-form-item>

				<el-form-item label="密码" prop="password">
					<el-input
						v-model="form.password"
						type="password"
						show-password
						autocomplete="current-password"
						placeholder="请输入密码"
						@keyup.enter="onSubmit"
					/>
				</el-form-item>

				<el-form-item>
					<el-button type="primary" :loading="loading" @click="onSubmit">登录</el-button>
				</el-form-item>
			</el-form>

			<el-alert
				v-if="successText"
				:title="successText"
				type="success"
				:closable="false"
				show-icon
			/>

			<el-alert
				v-if="errorText"
				:title="errorText"
				type="error"
				:closable="false"
				show-icon
			/>
		</el-card>
	</div>
</template>

<script lang="ts" setup>
import { reactive, ref } from 'vue';
import { ElMessage, type FormInstance, type FormRules } from 'element-plus';

interface LoginRequest {
	username: string;
	password: string;
}

interface LoginResponse {
	token: string;
}

const formRef = ref<FormInstance>();
const loading = ref(false);
const successText = ref('');
const errorText = ref('');
const form = reactive<LoginRequest>({
	username: '',
	password: ''
});

const rules: FormRules<LoginRequest> = {
	username: [{ required: true, message: '请输入用户名', trigger: 'blur' }],
	password: [{ required: true, message: '请输入密码', trigger: 'blur' }]
};

const apiBase = "http://localhost:8080"
const loginEndpoint = `${apiBase}/api/login`;

const onSubmit = async () => {
	successText.value = '';
	errorText.value = '';

	const isValid = await formRef.value?.validate().catch(() => false);
	if (!isValid) {
		return;
	}

	loading.value = true;
	try {
		const response = await fetch(loginEndpoint, {
			method: 'POST',
			headers: {
				'Content-Type': 'application/json',
			},
			body: JSON.stringify(form),
		});

		if (!response.ok) {
			const detail = await response.text();
			throw new Error(detail || `登录失败（HTTP ${response.status}）`);
		}

		const data = (await response.json()) as Partial<LoginResponse>;
		if (!data.token) {
			throw new Error('服务端未返回 token');
		}

		localStorage.setItem('jwt', data.token);
		successText.value = '登录成功，JWT 已保存到 localStorage（key: jwt）。';
		ElMessage.success('登录成功');
	} catch (error) {
		errorText.value = error instanceof Error ? error.message : '登录失败，请稍后重试。';
	} finally {
		loading.value = false;
	}
};
</script>

<style scoped>
.login-page {
	display: flex;
	justify-content: center;
	padding: 2rem 1rem;
}

.login-card {
	width: 100%;
	max-width: 420px;
}
</style>