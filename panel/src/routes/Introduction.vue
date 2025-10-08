<template>
    <h1>CSOL 集成工具</h1>
    此配置界面对应于集成工具版本 {{ VERSION }}，支持导入 1.4.1 及以上版本的配置文件，并导出为 {{ VERSION }} 版本工具使用的配置文件。
    <h2>功能概述</h2>
    <ul>
        <li>24 H 自定义武器挂机（防卡黄金僵尸奖励）</li>
        <li>挂机掉线自动重连</li>
        <li>配件自动合成</li>
        <li>商店自动重复购买</li>
    </ul>
    <h2>项目</h2>
    若本项目对您有帮助，可 star 一下本项目的仓库：
    <ul>
        <li>
            <a href="https://gitee.com/silver1867/csol-24-h" target="_blank">Gitee 仓库</a>
        </li>
        <li>
            <a href="https://github.com/UserNameUnavailableIsUnavailable/CSOL-Utilities" target="_blank">GitHub 仓库</a>
        </li>
    </ul>
    <blockquote>
        使用过程中如遇到任何问题，或有任何意见或建议，欢迎通过 ttyuig@126.com 或 admin@macrohard.fun 反馈。
        有开发经验者则建议通过 Gitee 或 GitHub 提交 issues。
    </blockquote>
    <h2>赞助（自愿）</h2>
    可通过如下方式赞助本项目：
    <div>
        <img src="/donate/wepay.avif" width="256px" />
        <img src="/donate/alipay.avif" width="256px" />
    </div>
</template>

<script lang="ts" setup>
import { inject } from 'vue';

const VERSION = inject("VERSION") as string;
</script>
