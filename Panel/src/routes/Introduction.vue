<template>
    <h1>CSOL 集成工具</h1>
    此配置界面对应于集成工具版本 <b style="color: red;">{{ VERSION }}</b>，支持导入 1.4.1 及以上版本的配置文件，并导出为 <b style="color: red;">{{ VERSION
        }}</b> 版本工具使用的配置文件。
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
    <h2 class="gold">赞助</h2>
    开发不易，若您觉得本项目对您有帮助，欢迎慷慨解囊：
    <div>
        <img src="/donate/wepay.avif" width="256px" />
        <img src="/donate/alipay.avif" width="256px" />
    </div>
</template>

<script lang="ts" setup>
import { inject } from 'vue';

const VERSION = inject("VERSION") as string;
</script>

<style>
.gold {
    font-family: 'Arial Black', sans-serif;
    color: #ffd700; /* 基础的金色 */
    text-shadow:
        /* 暗部阴影 - 模拟边缘的深色 */
        -1px -1px 0 #b38b00, /* 左上角深色边缘 */
        1px 1px 0 #8b6508,  /* 右下角更深的边缘 */

        /* 中间过渡色 - 增加厚度感 */
        2px 2px 2px #d4af37, /* 稍微模糊的过渡色 */
        3px 3px 3px #c09b2e,

        /* 高光阴影 - 模拟金属反射光 */
        -2px -2px 5px rgba(255, 255, 255, 0.6), /* 顶部和左侧的亮光 */
        -3px -3px 8px rgba(255, 255, 255, 0.4),

        /* 底部和右侧的微弱反光 */
        4px 4px 6px rgba(0, 0, 0, 0.3), /* 模拟底部阴影，增加立体感 */
        5px 5px 10px rgba(0, 0, 0, 0.2);
    /* 浏览器兼容性前缀可能需要，但text-shadow现代浏览器支持良好 */
}
</style>