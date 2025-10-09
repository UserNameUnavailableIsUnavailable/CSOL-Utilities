<template>
    <MessageBox title="通知" :text="notifyText" ok-text="确认" cancel-text="取消" :ok-callback="okCallback" :cancel-callback="cancelCallback" v-model:visible="showNotify" />
    <SectionWidget v-if="widget" :widget="widget" />
    <button class="import" @click="import_from_file?.click()">导入</button>
    <input type="file" ref="import_from_file" v-show="false" @change="import_setting($event)" />
    <button class="export" @click="export_setting">导出</button>
</template>

<script lang="ts" setup>
import { inject, provide, type Ref } from 'vue';
// @ts-ignore
import luaparse from "luaparse";
import * as LuaASTUtil from "../scripts/LuaASTUtil";
import SectionWidget from "../components/Setting/SectionWidget.vue";
import YAML from "js-yaml";
import { type SwitchWidget_T } from '../scripts/Widget';
import { SaveAs } from '../scripts/Utilities';
import { ref } from 'vue';
import { formatText } from 'lua-fmt';
import MessageBox from "../components/MessageBox.vue";

const notifyText = ref("存在未填写的必填项，是否仍然导出？");
const okCallback = ref(() => {});
const cancelCallback = ref(() => {});
const showNotify = ref(false);

const SETTING_SWITCHES: Map<string, Ref<string>> = new Map();
provide("SETTING_SWITCHES", SETTING_SWITCHES);
const SETTING_ITEM_STATES: Map<string, Ref<boolean>> = new Map();
provide("SETTING_ITEM_STATES", SETTING_ITEM_STATES);
const SETTING_ITEMS: Map<string, Ref<any>> = new Map();
provide("SETTING_ITEMS", SETTING_ITEMS);

let widget = ref<SwitchWidget_T | undefined>();
fetch("./GeneralSetting.yaml")
    .then(response => response.text())
    .then(data => {
        const w = YAML.load(data) as any;
        w.level = 1;
        widget.value = w;
    })
    .catch(error => console.error("Error fetching GeneralSetting.yaml.", error));

const VERSION = inject("VERSION") as string;

/// 导出设置。
function export_setting_impl() {
    let code = "if not __SETTING_LUA__ then\n" +
        "\t__SETTING_LUA__ = true\n" +
        `\tlocal __version__ = "${VERSION}"\n` +
        `\tInclude("Version.lua")\n` +
        `\tVersion:set("Setting", __version__)\n` +
        "\tSetting = {\n";

    SETTING_ITEMS.forEach((v, k) => {
        if (v.value === "") {
            code += `\t\t${k} = nil,\n`;
        } else {
            code += `\t\t${k} = ${v.value},\n`;
        }
    });
    code += "\t}\n";
    code += "end -- __SETTING_LUA__\n";
    code = formatText(code)
    console.log(code);
    try {
        SaveAs("Setting.lua", code);
    } catch(e) {
    }
}

/**
 * 从 Lua 抽象语法树中导入设置，并将设置填入配置面板。
 * @param ast Lua 抽象语法树
 * @returns 从抽象语法树中解析得到的键值对形式的设置
 */
function ResolveSetting(ast: any): Map<string, string> {
    // 广度优先遍历找到 Setting 的定义
    let settings = new Map<string, string>();
    if (!ast) {
        return settings;
    }
    let settings_node;
    let queue = new Array();
    // @ts-ignore
    ast.body.forEach(e => {
        queue.push(e);
    })
    while (queue.length > 0) {
        let node = queue.shift();
        if (node.type === LuaASTUtil.AST_NODE_ASSIGNMENT_STATEMENT) {
            let index = 0;
            while (index < node.variables.length) {
                let variable = LuaASTUtil.GenerateLuaCodeFromAST(node.variables[index], 0);
                if (variable === "Setting") {
                    settings_node = node.init[index];
                    break;
                }
                index++;
            }
        }
        // 采取非递归广度优先遍历，同级节点入队
        for (const property in node) {
            let subnode = node[property] as Object;
            if (Array.isArray(subnode)) {
                subnode.forEach(e => {
                    if (e.hasOwnProperty("type")) {
                        queue.push(e);
                    }
                })
            } else if (subnode?.hasOwnProperty("type")) {
                queue.push(subnode);
            }
        }
    }
    if (settings_node.type !== LuaASTUtil.AST_NODE_TABLE_CONSTRUCTOR_EXPRESSION) {
        throw `Assertion Failed: settings_node.type !== AST_NODE_TABLE_CONSTRUCTOR_EXPRESSION`;
    }
    // @ts-ignore
    settings_node.fields.forEach(e => {
        LuaASTUtil.SetIndentUnit('');
        settings.set(
            LuaASTUtil.GenerateLuaCodeFromAST(e.key, 0),
            LuaASTUtil.GenerateLuaCodeFromAST(e.value, 0)
        );
        LuaASTUtil.SetIndentUnit('\t');
    })
    return settings;
}


const import_from_file = ref<HTMLInputElement>();

async function import_setting(event: Event) {
    const input = event.target as HTMLInputElement;
    const file = input.files?.[0];
    if (file) {
        const reader = new FileReader();
        reader.onload = (e) => {
            const content = e.target?.result;
            let ast;
            try {
                // @ts-ignore
                ast = luaparse.parse(content);
            } catch (e) {
                alert("无法解析指定文件，可能是文件类型非法或存在语法错误。")
            }
            const setting = ResolveSetting(ast);
            console.log(setting);
            setting.forEach((v, k) => {
                if (SETTING_ITEMS.has(k)) {
                    const rv = SETTING_ITEMS.get(k) as Ref<string>;
                    rv.value = v;
                }
            });
        }
        reader.readAsText(file);
        input.value = ""; // 重置，允许重复导入同一个文件
    }
}

const export_setting = () => {
    const empty_field_ids: string[] = [];
    SETTING_ITEMS.forEach((v, k) => {
        if (v.value === "" && SETTING_ITEM_STATES.get(k)?.value) { // 激活的字段
            empty_field_ids.push(k);
        }
    });
    
    if (empty_field_ids.length > 0) {
        notifyText.value = `存在未填写的必填项：${empty_field_ids.join(", ")}, 是否仍然导出?`;
        const glimmer = () => {
            let isFirst = true;
            empty_field_ids.forEach(id => {
                const element = document.getElementById(id);
                if (element) { // 只滚动到第一个
                    if (isFirst) {
                        element.scrollIntoView({
                            behavior: "smooth",
                            block: "center"
                        });
                    }
                    isFirst = false;
                    element.classList.add('Glimmer'); // 添加闪烁动画
                    setTimeout(() => {
                        element?.classList.remove("Glimmer")
                    }, 2500);
                }
            });
        };
        okCallback.value = () => {
            export_setting_impl();
            glimmer();
        };
        cancelCallback.value = () => {
            glimmer();
        };
        showNotify.value = true; // 显示提示框
    } else {
        export_setting_impl();
    }
};
</script>

<style lang="scss">
@use "../styles/basic.scss";
@keyframes Glimmer {
    0% {
        box-shadow: 0 0 0px 0px rgba(255, 80, 80, 0.0);
        background-color: #fff8f8;
        transform: scale(1);
    }
    30% {
        box-shadow: 0 0 12px 4px rgba(255, 80, 80, 0.5);
        background-color: #ffeaea;
        transform: scale(1.03);
    }
    70% {
        box-shadow: 0 0 24px 8px rgba(255, 80, 80, 0.7);
        background-color: #ffeaea;
        transform: scale(1.05);
    }
    100% {
        box-shadow: 0 0 0px 0px rgba(255, 80, 80, 0.0);
        background-color: #fff8f8;
        transform: scale(1);
    }
}

.Glimmer {
    animation: Glimmer 2.5s cubic-bezier(.4,0,.2,1);
    transition: border 0.2s, box-shadow 0.2s, background-color 0.2s;
}
</style>
