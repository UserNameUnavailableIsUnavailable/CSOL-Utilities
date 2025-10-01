import * as LuaASTUtil from "./LuaASTUtil";

export const VERTICAL_STRAFE_MODES = [
    { description: "无", content: "\"none\"" },
    { description: "固定向上", content: "\"up\"" },
    { description: "固定向下", content: "\"down\"" },
    { description: "随机", content: "\"random\"" },
    { description: "简谐振动（上下交替）", content: "\"oscillating\"" }
];

export const HORIZONTAL_STRAFE_MODES = [
    { description: "无", content: "\"none\"" },
    { description: "固定向左", content: "\"left\"" },
    { description: "固定向右", content: "\"right\"" },
    { description: "随机", content: "\"random\"" },
    { description: "简谐振动（左右交替）", content: "\"oscillating\"" }
];

export type AttackButton_T = "Mouse.LEFT" | "Mouse.RIGHT"; // 攻击按键类型
export const AttackButton_T = {
    LEFT: "Mouse.LEFT" as "Mouse.LEFT", // 左键
    RIGHT: "Mouse.RIGHT" as "Mouse.RIGHT" // 右键
};

export type WeaponNumber_T = "Weapon.NULL" | "Weapon.PRIMARY" | "Weapon.SECONDARY" | "Weapon.MELEE" | "Weapon.GRENADE";
export const WeaponNumber_T = {
    NULL: "Weapon.NULL" as "Weapon.NULL",
    PRIMARY: "Weapon.PRIMARY" as "Weapon.PRIMARY",
    SECONDARY: "Weapon.SECONDARY" as "Weapon.SECONDARY",
    MELEE: "Weapon.MELEE" as "Weapon.MELEE",
    GRENADE: "Weapon.GRENADE" as "Weapon.GRENADE"
};

export const TEMPLATE_URL = "https://www.macrohard.fun/CSOL-Utilities/panel/weapon_templates/listing.yaml"; // 武器模板列表文件 URL

export type Weapon_T =  {
    name: string // 名称
    purchase_sequence: string // 购买按键序列
};

export type Armor_T = Weapon_T; // 护甲

export type PartWeapon_T = Weapon_T; // 配件武器

export type ConventionalWeapon_T = Weapon_T & {
    template_name: string; // 代码模板名称
    attack_button: AttackButton_T; // 攻击按键
    number: WeaponNumber_T; // 武器栏位（主武器、副武器、近战、手雷）
    switch_delay: number; // 切枪延迟
    horizontal_strafe_mode: string; // 水平扫射方向
    vertical_strafe_mode: string; // 垂直扫射方向
};

export type SpecialWeapon_T = Weapon_T & {
    template_name: string; // 代码模板名称
};

// 将 Lua AST 中的 TableCall 表达式转换为规范的 CallExpression
function ConvertTableCallExpressionToCallExpression(node: any) {
    if (node.type === LuaASTUtil.AST_NODE_CALL_EXPRESSION) {
        return node
    }
    if (node.type === LuaASTUtil.AST_NODE_TABLE_CALL_EXPRESSION) {
        return {
            type: LuaASTUtil.AST_NODE_CALL_EXPRESSION,
            base: node.base,
            arguments: [node.argument]
        }
    } else {
        throw Error(`Failed to convert node to AST_NODE_CALL_EXPRESSION: illegal node type.\nNode:${node}`)
    }

}

// 解析武器，将其转换为字典
export function ResolveWeapon(ast: any, weapon: Record<string, string>) {
    if (ast.type !== LuaASTUtil.AST_NODE_CALL_EXPRESSION) {
        throw Error(`Invalid AST node type: ${ast.type}, AST_NODE_CALL_EXPRESSION expected.`);
    }
    const function_name = LuaASTUtil.GenerateLuaCodeFromAST(ast.base);
    if (function_name !== "Weapon:new") {
        throw(`Unknown function: ${function_name}, Weapon:new expected.`);
    }
    if (ast.arguments.length > 1) {
        throw("Weapon:new accepts exactly one argument.");
    }
    let args = ast.arguments[0]; // 武器参数列表
    for (const field of args.fields) {
        const key = LuaASTUtil.GenerateLuaCodeFromAST(field.key); // 字段名
        const value = LuaASTUtil.GenerateLuaCodeFromAST(field.value); // 字段值
        weapon[key] = value;
    }
}

export function GenerateWeaponCode(weapon: Record<string, string>) {
    const items = new Array<string>();
    for (const k in weapon) {
        const v = weapon[k];
        if (typeof k === "string" && typeof v === "string")
        items.push(`${k} = ${weapon[k]}`);
    }
    return `Weapon:new({${items.join(",\n")}})`;
}

export function ResolveWeaponList(ast: any) {
    let armor: Record<string, string>|null = null;
    const default_part_weapons = new Array<Record<string, string>>();
    const extended_part_weapons = new Array<Record<string, string>>();
    const default_conventional_weapons = new Array<Record<string, string>>();
    const extended_conventional_weapons = new Array<Record<string, string>>();
    const default_special_weapons = new Array<Record<string, string>>();
    const extended_special_weapons = new Array<Record<string, string>>();

    // 广度优先遍历，将语法树的每个节点加入队列
    const queue = new Array()
    // @ts-ignore
    ast.body.forEach(e => {
        queue.push(e);
    });

    // 广度优先访问每个节点
    while (queue.length > 0) {
        let node = queue.shift()
        if (node.type === LuaASTUtil.AST_NODE_ASSIGNMENT_STATEMENT) { // 检查赋值语句，筛选出关心的赋值语句
            let index = 0
            /* 遍历节点，将 Table Call Expression 转换为 Call Expression */
            while (index < node.variables.length) {
                let variable_name = LuaASTUtil.GenerateLuaCodeFromAST(node.variables[index], 0)
                if (variable_name === "Armor" || variable_name === "AC") {
                    const weapon_new_call = ConvertTableCallExpressionToCallExpression(node.init[index]) ;
                    armor = {};
                    ResolveWeapon(weapon_new_call, armor);
                } else if (variable_name === "DefaultPartWeapons" || variable_name === "PartWeaponList") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        const weapon_new_call = ConvertTableCallExpressionToCallExpression(weapons[i].value);
                        const weapon: Record<string, string> = {};
                        ResolveWeapon(weapon_new_call, weapon);
                        default_part_weapons.push(weapon);
                    }
                } else if (variable_name === "DefaultConventionalWeapons" || variable_name === "DefaultWeaponList") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        const weapon_new_call = ConvertTableCallExpressionToCallExpression(weapons[i].value);
                        const weapon: Record<string, string> = {};
                        ResolveWeapon(weapon_new_call, weapon);
                        if (weapon["template_name"] || weapon["template_code"]) {
                            default_conventional_weapons.push(weapon);
                        } else {
                            const weapon_new_call = ConvertTableCallExpressionToCallExpression(weapons[i].value);
                            const weapon: Record<string, string> = {};
                            ResolveWeapon(weapon_new_call, weapon);
                            default_conventional_weapons.push(weapon);
                        }
                    }
                } else if (variable_name === "DefaultSpecialWeapons") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        const weapon_new_call = ConvertTableCallExpressionToCallExpression(weapons[i].value);
                        const weapon: Record<string, string> = {};
                        ResolveWeapon(weapon_new_call, weapon);
                        default_special_weapons.push(weapon);
                    }
                } else if (variable_name === "ExtendedPartWeapons") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        const weapon_new_call = ConvertTableCallExpressionToCallExpression(weapons[i].value);
                        const weapon: Record<string, string> = {};
                        ResolveWeapon(weapon_new_call, weapon);
                        extended_part_weapons.push(weapon);
                    }
                } else if (variable_name === "ExtendedConventionalWeapons" || variable_name === "ExtendedWeaponList") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        const weapon_new_call = ConvertTableCallExpressionToCallExpression(weapons[i].value)
                        const weapon: Record<string, string> = {};
                        ResolveWeapon(weapon_new_call, weapon);
                        if (weapon["template_name"] || weapon["template_code"]) {
                            extended_conventional_weapons.push(weapon);
                        } else {
                            extended_conventional_weapons.push(weapon);
                        }
                    }
                } else if (variable_name === "ExtendedSpecialWeapons") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        const weapon_new_call = ConvertTableCallExpressionToCallExpression(weapons[i].value)
                        const weapon: Record<string, string> = {};
                        ResolveWeapon(weapon_new_call, weapon);
                        extended_special_weapons.push(weapon)
                    }
                } else if (variable_name === "SpecialWeapon") {
                    const weapon_new_call = ConvertTableCallExpressionToCallExpression(node.init[index]);
                    let weapon: Record<string, string> = {};
                    ResolveWeapon(weapon_new_call, weapon);
                    extended_special_weapons.push(weapon);
                }
                index++;
            }
        }
        for (const property in node) {
            let subnode = node[property]
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
    return {
        armor: armor,
        default_part_weapons: default_part_weapons,
        default_conventional_weapons: default_conventional_weapons,
        default_special_weapons:default_special_weapons,
        extended_part_weapons: extended_part_weapons,
        extended_conventional_weapons: extended_conventional_weapons,
        extended_special_weapons: extended_special_weapons
    }
}
