<template>
    <button class="import" @click="import_from_file?.click()">导入</button>
    <input type="file" ref="import_from_file" v-show="false" @change="import_weapon_list($event)" />
    <button class="export" @click="export_weapon_list">导出</button>
    <h1>武器列表</h1>
    <ul>
        <li>
            本配置界面用于导入、导出集成工具 Executor 目录下的 WeaponList.lua 文件；
        </li>
        <li>
            集成工具支持两种挂机模式：默认模式和扩展模式。在本配置面板中，可以同时针对这两种挂机模式进行配置，最后以 Lua 源码形式导出一份武器列表配置文件（即 WeaponList.lua），其中包含对默认模式和扩展模式的配置；
        </li>
        <li>
            定制武器的模板会时常更新。
        </li>
    </ul>
    <h2>挂机模式</h2>
    <p>选择需要配置的挂机模式，两种模式共享护甲配置，其余武器装备的配置独立。</p>

    <input type="radio" value="default" :checked="tag==='default'" @click="tag='default'" /><span>配置默认挂机模式</span>
    <input type="radio" value="extended" :checked="tag==='extended'" @click="tag='extended'" /><span>配置扩展挂机模式</span>

    <h2>护甲</h2>
    <div v-if="armor" class="weapon-widget">
        <NullWeapon v-model:fields="armor" @update:fields="dirty++" :remarks="armor_remarks" />
        <button class="widget-button" v-if="armor" @click="delete_armor()">删除</button>
    </div>
    <button class="widget-button" v-if="!armor" @click="add_armor()">添加</button>
    
    <div v-show="tag==='default'">
        <h2>配件武器</h2>
        <div class="weapon-widget" v-for="w, id in default_part_weapons" :key="id">
            <NullWeapon :fields="unref(w)" @update:fields="update_weapon(id, $event, default_part_weapons)" :remarks="part_weapon_remarks" />
            <button class="widget-button" @click="remove_weapon(id, default_part_weapons)">删除</button>
        </div>
        <button class="widget-button" @click="add_part_weapon(default_part_weapons)">添加</button>

        <h2>常规武器</h2>
        <div class="weapon-widget" v-for="w, id in default_conventional_weapons" :key="id">
            <CustomizedWeapon v-if="unref(w)['template_name']" :fields="unref(w)" @update:fields="update_weapon(id, $event, default_conventional_weapons)"  type="Conventional" :remarks="customized_weapon_remarks" language="zh_CN" />
            <NormalWeapon v-else :fields="unref(w)" @update:fields="update_weapon(id, $event, default_conventional_weapons)" :remarks="normal_weapon_remarks" />
            <button class="widget-button" @click="remove_weapon(id, default_conventional_weapons)">删除</button>
        </div>
        <button class="widget-button" @click="add_conventional_weapon(false, default_conventional_weapons)">添加（生成普通代码）</button>
        <button class="widget-button" @click="add_conventional_weapon(true, default_conventional_weapons)">新增（生成定制代码）</button>

        <h2>特殊武器</h2>
        <div class="weapon-widget" v-for="w, id in default_special_weapons" :key="id">
            <CustomizedWeapon :fields="unref(w)" @update:fields="update_weapon(id, $event, default_special_weapons)" type="Special" :remarks="customized_weapon_remarks" language="zh_CN" />
            <button class="widget-button" @click="remove_weapon(id, default_special_weapons)">删除</button>
        </div>
        <button class="widget-button" @click="add_special_weapon(default_special_weapons)">添加</button>

    </div>
    <div v-show="tag==='extended'">
        <h2>配件武器</h2>
        <div class="weapon-widget" v-for="w, id in extended_part_weapons" :key="id">
            <NullWeapon :fields="unref(w)" @update:fields="update_weapon(id, $event, extended_part_weapons)" :remarks="part_weapon_remarks" />
            <button class="widget-button" @click="remove_weapon(id, extended_part_weapons)">删除</button>
        </div>
        <button class="widget-button" @click="add_part_weapon(extended_part_weapons)">添加</button>

        <h2>常规武器</h2>
        <div class="weapon-widget" v-for="w, id in extended_conventional_weapons" :key="id">
            <CustomizedWeapon v-if="unref(w)['template_name']" :fields="unref(w)" @update:fields="update_weapon(id, $event, extended_conventional_weapons)"  type="Conventional" :remarks="customized_weapon_remarks" language="zh_CN" />
            <NormalWeapon v-else :fields="unref(w)" @update:fields="update_weapon(id, $event, extended_conventional_weapons)" :remarks="normal_weapon_remarks" />
            <button class="widget-button" @click="remove_weapon(id, extended_conventional_weapons)">删除</button>
        </div>
        <button class="widget-button" @click="add_conventional_weapon(false, extended_conventional_weapons)">添加（生成普通代码）</button>
        <button class="widget-button" @click="add_conventional_weapon(true, extended_conventional_weapons)">新增（生成定制代码）</button>

        <h2>特殊武器</h2>
        <div class="weapon-widget" v-for="w, id in extended_special_weapons" :key="id">
            <CustomizedWeapon v-bind:fields="unref(w)" @update:fields="update_weapon(id, $event, extended_special_weapons)" type="Special" :remarks="customized_weapon_remarks" language="zh_CN" />
            <button class="widget-button" @click="remove_weapon(id, extended_special_weapons)">删除</button>
        </div>
        <button class="widget-button" @click="add_special_weapon(extended_special_weapons)">添加</button>
    </div>
</template>

<style lang="scss" scoped>
@use "../styles/basic.scss";
.weapon-widget {
    outline: 0.5rem solid gray;
    border-radius: 10px;
    padding: 5px;
    margin: 20px;
}
.widget-button {
    background-color: #585756;
    color: white;
    border-radius: 5px;
    padding: 10px 20px;
}
.widget-button:hover {
    background-color: #747171;
}
</style>


<script lang="ts" setup>
import { inject, onMounted, reactive, ref, type Ref, unref } from 'vue';
import luaparse from "luaparse";
import NullWeapon from '../components/WeaponList/NullWeapon.vue';
import NormalWeapon from '../components/WeaponList/NormalWeapon.vue';
import CustomizedWeapon from '../components/WeaponList/CustomizedWeapon.vue';
import { GenerateWeaponCode, ResolveWeaponList } from '../scripts/Weapon';
import { formatText } from 'lua-fmt';
import { SaveAs } from '../scripts/Utilities';

const tag = ref("default");

const armor_remarks = [
    "J 键可切换 T / CT 阵营武器购买界面。",
    "若装备购买界面中没有护甲，可将护甲购买序列设置为：<code class=\"language-plain\">B R</code>。",
];

const part_weapon_remarks = [
    "J 键可切换 T / CT 阵营武器购买界面。",
    "配件武器不参与攻击，仅使用其增益效果（如安装生命配件、伤害配件的武器）。",
];

const normal_weapon_remarks = [
    "J 键可切换 T / CT 阵营武器购买界面。",
    "攻击持续时间取值一般在 7.5 ~ 20 秒，武器消耗完一个弹夹的时间可作为选择依据（对于近战武器以及没有传统意义上弹夹的武器，此处的攻击时间可适当延长，但不建议超过 30 秒）。",
    "扫射方向即使用武器攻击时视角的运动方向，分解为水平和垂直两个方向。水平方向一般选为随机，垂直方向按实际需求进行调整。例如，刷狂戮巨蚊、鹞子风筝时可选为固定向上。",
];

const customized_weapon_remarks = [
    "J 键可切换 T / CT 阵营武器购买界面。",
    "攻击持续时间取值一般在 7.5 ~ 20 秒，武器消耗完一个弹夹的时间可作为选择依据（对于近战武器以及没有传统意义上弹夹的武器，此处的攻击时间可适当延长，但不建议超过 30 秒）。",
    "对于部分定制类武器（尤其是 4 号位武器），每轮攻击持续时间、扫射方向不会生效。",
    "扫射方向即使用武器攻击时视角的运动方向，分解为水平和垂直两个方向。水平方向一般选为随机，垂直方向按实际需求进行调整。例如，刷狂戮巨蚊、鹞子风筝时可选为固定向上。",
];

type WeaponFields = Record<string, string>;
type WeaponList = Record<number, Ref<WeaponFields>>;

// 武器 id
let weapon_id = 0;
const dirty = ref(0);
// 护甲
const armor = ref<WeaponFields|null>();

function add_armor() {
    armor.value = {
        name: "\"防弹衣+头盔\"",
        purchase_sequence: "{}"
    };
    dirty.value++;
}

function delete_armor() {
    armor.value = null;
    dirty.value++;
}

// 配件武器
const default_part_weapons = ref<WeaponList>({});
const extended_part_weapons = ref<WeaponList>({});

function add_part_weapon(list: WeaponList) {
    const id = weapon_id++;
    const weapon: WeaponFields = {};
    weapon["name"] = "\"\"";
    weapon["purchase_sequence"] = "{}";
    list[id] = ref(weapon);
    dirty.value++;
}

// 常规武器
const default_conventional_weapons = ref<WeaponList>({});
const extended_conventional_weapons = ref<WeaponList>({});

function add_conventional_weapon(customized: boolean, list: WeaponList) {
    const id = weapon_id++;
    const weapon = reactive<WeaponFields>({});
    if (!customized) {
        weapon["name"] = "\"\"";
        weapon["purchase_sequence"] = "{}";
        weapon["number"] = "Weapon.PRIMARY";
        weapon["switch_delay"] = "100";
        weapon["attack_button"] = "Mouse.LEFT";
    } else {
        weapon["name"] = "\"\"";
        weapon["purchase_sequence"] = "{}";
        weapon["template_name"] = "nil";
    }
    list[id] = ref(weapon);
    dirty.value++;
}

// 特殊武器
const default_special_weapons = ref<WeaponList>({});
const extended_special_weapons = ref<WeaponList>({});


function add_special_weapon(list: WeaponList) {
    const id = weapon_id++;
    const weapon = reactive<WeaponFields>({});
    weapon["name"] = "\"\"";
    weapon["purchase_sequence"] = "{}";
    weapon["template_name"] = "nil";
    list[id] = ref(weapon);
    dirty.value++;
}

function remove_weapon(id: number, list: WeaponList) {
    delete list[id];
    dirty.value++;
}

function update_weapon(id: number, wd: WeaponFields, list: WeaponList) {
    if (list[id]) {
        list[id].value = wd;
        dirty.value++;
    }
}

function generate_weapon_list_code(weapons: WeaponList) {
    let code = "{\n";
    for (const id in weapons) {
        code += GenerateWeaponCode(unref(weapons[id])) + ",\n";
    }
    code += "}\n";
    return code;
}

const VERSION = inject("VERSION") as string;

let preview_code = ref(false);
onMounted(() => {
    preview_code.value = true;
});

function import_weapon_list(event: Event) {
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
                alert("无法解析指定文件，可能是文件类型非法或存在语法错误");
            }
            const weapon_list = ResolveWeaponList(ast);
            armor.value = weapon_list.armor;
            // default
            const dpw: WeaponList = {};
            for (const k in weapon_list.default_part_weapons) {
                dpw[weapon_id++] = ref(weapon_list.default_part_weapons[k]);
            }
            default_part_weapons.value = dpw;
            const dcw: WeaponList = {};
            for (const k in weapon_list.default_conventional_weapons) {
                dcw[weapon_id++] = ref(weapon_list.default_conventional_weapons[k]);
            }
            default_conventional_weapons.value = dcw;
            const dsw: WeaponList = {};
            for (const k in weapon_list.default_special_weapons) {
                dsw[weapon_id++] = ref(weapon_list.default_special_weapons[k]);
            }
            default_special_weapons.value = dsw;
            const epw: WeaponList = {};
            for (const k in weapon_list.extended_part_weapons) {
                epw[weapon_id++] = ref(weapon_list.extended_part_weapons[k]);
            }
            // extended
            extended_part_weapons.value = epw;
            const ecw: WeaponList = {};
            for (const k in weapon_list.extended_conventional_weapons) {
                ecw[weapon_id++] = ref(weapon_list.extended_conventional_weapons[k]);
            }
            extended_conventional_weapons.value = ecw;
            const esw: WeaponList = {};
            for (const k in weapon_list.extended_special_weapons) {
                esw[weapon_id++] = ref(weapon_list.extended_special_weapons[k]);
            }
            extended_special_weapons.value = esw;
            dirty.value++;
        }
        reader.readAsText(file);
        input.value = "";
    }
}

function export_weapon_list() {
    const blocks = new Array<string>();
    const a = unref(armor);
    if (a) {
        blocks.push(`Armor = ${GenerateWeaponCode(a)}\n`);
    }
    // default
    blocks.push(`DefaultPartWeapons = ${generate_weapon_list_code(unref(default_part_weapons))}`);
    blocks.push(`DefaultConventionalWeapons = ${generate_weapon_list_code(unref(default_conventional_weapons))}`);
    blocks.push(`DefaultSpecialWeapons = ${generate_weapon_list_code(default_special_weapons.value)}`);
    // extended
    blocks.push(`ExtendedPartWeapons = ${generate_weapon_list_code(unref(extended_part_weapons))}`);
    blocks.push(`ExtendedConventionalWeapons = ${generate_weapon_list_code(unref(extended_conventional_weapons))}`);
    blocks.push(`ExtendedSpecialWeapons = ${generate_weapon_list_code(unref(extended_special_weapons))}`);
    const ret = `if not __WEAPON_LIST_LUA__ then\n` +
    `\t__WEAPON_LIST_LUA__ = true\n` +
    `\tlocal __version__ = "${VERSION}"\n` +
    `\tInclude("Version.lua")\n` +
    `\tVersion:set("WeaponList", __version__)\n` +
    `\tVersion:require("WeaponList", "Weapon", "v1.5.4")\n` +
    `\t${blocks.join("\n")}\n` +
    `end -- __WEAPON_LIST_LUA__\n`;
    console.log(ret);
    const code = formatText(ret);
    SaveAs("WeaponList.lua", code);
}

const import_from_file = ref();
</script>
