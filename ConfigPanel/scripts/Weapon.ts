declare var luaparse
declare var g_WeaponTemplateList

import * as LuaASTUtil from "./LuaASTUtil.js"

export class Weapon {
    static s_Id: number = 0
    m_Id: number
    m_Name: string
    m_PurchaseSequence: string[]
    m_Number: string // 武器序号：Weapon.NULL（配件武器或护甲）| Weapon.PRIMARY（主武器）| Weapon.SECONDARY（副武器）| Weapon.MELEE（近身武器）| Weapon.GRENADE（手雷）
    m_NameElement: HTMLInputElement
    m_PurchaseSequenceElement: HTMLInputElement
    m_DeleteButtonElement: HTMLButtonElement
    m_Element: HTMLElement
    static GenerateId(): number {
        return Weapon.s_Id++
    }
    constructor() {
        this.m_Id = Weapon.GenerateId()
        this.m_Name = "未定义"
        this.m_Number = "Weapon.NULL"
        this.m_PurchaseSequence = new Array<string>()
        this.m_Element = document.createElement("div")
        this.m_Element.id = `Weapon_${this.m_Id}`
        let p: HTMLParagraphElement
        let input: HTMLInputElement
        let label: HTMLLabelElement
        let button: HTMLButtonElement
        // name
        p = document.createElement('p')
        label = document.createElement("label")
        label.innerHTML = "武器/装备名称"
        p.appendChild(label)
        this.m_NameElement = input = document.createElement("input")
        input.id = `Weapon_${this.m_Id}_name`
        input.type = "text"
        p.appendChild(input)
        this.m_Element.appendChild(p)
        // purchase_sequence, read-only
        p = document.createElement('p')
        label = document.createElement("label")
        label.innerHTML = "购买按键序列"
        p.appendChild(label)
        this.m_PurchaseSequenceElement = input = document.createElement("input")
        input.id = `Weapon_${this.m_Id}_purchase_sequence_display`
        input.type = "text"
        input.readOnly = true
        p.appendChild(input)
        button = document.createElement("button")
        button.id = `Weapon_${this.m_Id}_purchase_sequence_record`
        button.textContent = "⏺️"
        button.onclick = () => { this.start_recording() }
        p.appendChild(button)
        this.m_Element.appendChild(p)
    }
    static s_KEYMAP: Map<string, string> // A: Keyboard.A
    static s_REVERSE_KEYMAP: Map<string, string> // Keyboard.A: A
    static s_Recording = false
    static s_Recorder: Weapon | undefined = undefined
    static s_KeyStrokes: string[] = new Array<string>()
    static {
        Weapon.s_KEYMAP = new Map<string, string>()
        Weapon.s_REVERSE_KEYMAP = new Map<string, string>()
        Weapon.s_KEYMAP.set('0', "Keyboard.ZERO")
        Weapon.s_KEYMAP.set('1', "Keyboard.ONE")
        Weapon.s_KEYMAP.set('2', "Keyboard.TWO")
        Weapon.s_KEYMAP.set('3', "Keyboard.THREE")
        Weapon.s_KEYMAP.set('4', "Keyboard.FOUR")
        Weapon.s_KEYMAP.set('5', "Keyboard.FIVE")
        Weapon.s_KEYMAP.set('6', "Keyboard.SIX")
        Weapon.s_KEYMAP.set('7', "Keyboard.SEVEN")
        Weapon.s_KEYMAP.set('8', "Keyboard.EIGHT")
        Weapon.s_KEYMAP.set('9', "Keyboard.NINE")
        let A = 'A'.charCodeAt(0)
        let Z = 'Z'.charCodeAt(0)
        for (let i = A; i <= Z; i++) {
            let c = String.fromCharCode(i)
            Weapon.s_KEYMAP.set(c, `Keyboard.${c}`)
        }
        Weapon.s_KEYMAP.forEach((v, k) => {
            this.s_REVERSE_KEYMAP.set(v, k)
        })
    }
    static OnKeystroke(kbd_event: KeyboardEvent) {
        if (kbd_event.key.length !== 1) {
            return
        }
        let key = kbd_event.key.toUpperCase()
        let ZERO = '0'.charCodeAt(0)
        let NINE = '9'.charCodeAt(0)
        let A = 'A'.charCodeAt(0)
        let Z = 'Z'.charCodeAt(0)
        let key_ascii = key.charCodeAt(0)
        if (key_ascii >= ZERO && key_ascii <= NINE || key_ascii >= A && key_ascii <= Z) {
            Weapon.s_KeyStrokes.push(key)
            Weapon.s_Recorder.m_PurchaseSequence.push(Weapon.s_KEYMAP.get(key))
            let display = document.getElementById(`Weapon_${Weapon.s_Recorder.m_Id}_purchase_sequence_display`) as HTMLInputElement
            display.value = Weapon.s_KeyStrokes.join(" ")
        }
    }
    stop_recording() {
        let weapon_id = `Weapon_${this.m_Id}`
        if (!Weapon.s_Recording || Weapon.s_Recorder != this) { // 当前未在录制
            return
        }
        let button = document.getElementById(weapon_id + "_purchase_sequence_record")
        let display = document.getElementById(weapon_id + "_purchase_sequence_display")
        if (!button || !display) {
            return
        }
        window.removeEventListener("keyup", Weapon.OnKeystroke)
        button.textContent = "⏺️"
        button.onclick = () => { this.start_recording() }
        Weapon.s_Recording = false
        Weapon.s_Recorder = undefined
        Weapon.s_KeyStrokes = new Array<string>()
        console.log(this.m_PurchaseSequence)
    }
    start_recording() {
        let weapon_id = `Weapon_${this.m_Id}`
        if (Weapon.s_Recording) {
            if (this != Weapon.s_Recorder) { // 当前有其他武器在录制
                alert("请先结束当前录制！")
            }
            return // 所选武器已经在录制
        }
        let button = document.getElementById(weapon_id + "_purchase_sequence_record")
        let display = document.getElementById(weapon_id + "_purchase_sequence_display")
        if (!button || !display) {
            return
        }
        this.m_PurchaseSequence = new Array<string>()
        this.m_PurchaseSequenceElement.value = ""
        window.addEventListener("keyup", Weapon.OnKeystroke)
        button.textContent = "⏹️️"
        button.onclick = () => { this.stop_recording() }
        Weapon.s_Recording = true
        Weapon.s_Recorder = this
    }
    append_remove_button(onremoval: () => any | undefined = undefined) {
        // 在组件中添加删除按钮
        let p = document.createElement('p')
        let button = document.createElement("button")
        button.textContent = "删除"
        button.id = `Weapon_${this.m_Id}_remove`
        button.onclick = () => {
            this.stop_recording() // 停止可能正在进行的录制操作
            if (onremoval) onremoval() // 执行回调
        }
        p.appendChild(button)
        this.m_Element.appendChild(p)
        this.m_DeleteButtonElement = button
    }
    gather() {
        let name = document.getElementById(`Weapon_${this.m_Id}_name`) as HTMLInputElement
        this.m_Name= name.value
    }
    /**
     * 生成武器源代码
     */
    generate(indent: string, indent_level: number = 0, first_line_indent: boolean = true): string {
        this.gather()
        return `${first_line_indent ? indent.repeat(indent_level) : ""}Weapon:new{\n` +
        `${indent.repeat(indent_level + 1)}name = "${this.m_Name}",\n` +
        `${indent.repeat(indent_level + 1)}number = ${this.m_Number},\n` +
        `${indent.repeat(indent_level + 1)}purchase_sequence = { ${this.m_PurchaseSequence.join(", ")} }\n` +
        `${indent.repeat(indent_level)}}`
    }
    /**
     * 修改某些字段后，更新 HTML 元素。注意，m_Element 必须已经加入 DOM，否则不会执行任何操作。
     */
    async update(): Promise<void> {
        // name
        let name = document.getElementById(`Weapon_${this.m_Id}_name`) as HTMLInputElement
        name.value = this.m_Name
        // purchase_sequence
        let purchase_sequence = document.getElementById(`Weapon_${this.m_Id}_purchase_sequence_display`) as HTMLInputElement
        purchase_sequence.value = this.m_PurchaseSequence.map(e => { return Weapon.s_REVERSE_KEYMAP.get(e) }).join(' ')
    }
}

export class PartWeapon extends Weapon {
}

export class Armor extends Weapon {
    constructor() {
        super()
        this.m_NameElement.value = "防弹衣+头盔"
    }
    override generate(indent: string, indent_level: number = 0, first_line_indent: boolean = true): string {
        this.gather()
        return `${first_line_indent ? indent.repeat(indent_level) : ""}Weapon:new{\n` +
        `${indent.repeat(indent_level + 1)}name = "${this.m_Name}",\n` +
        `${indent.repeat(indent_level + 1)}purchase_sequence = { ${this.m_PurchaseSequence.join(", ")} }\n` +
        `${indent.repeat(indent_level)}}`
    }
}

export class ConventionalWeapon extends Weapon {
    m_SwitchDelay: string // 切枪延迟，单位为毫秒
    m_AttackButton: string // 攻击按钮：Mouse.LEFT | Mouse.RIGHT
    m_SwitchDelayElement: HTMLInputElement
    m_AttackButtonElements: Array<HTMLInputElement>
    m_NumberElement: HTMLSelectElement
    constructor() {
        super()
        let p: HTMLParagraphElement
        let label: HTMLLabelElement
        let input: HTMLInputElement
        let select: HTMLSelectElement
        let option: HTMLOptionElement
        // 武器栏位
        p = document.createElement('p')
        label = document.createElement("label")
        label.innerHTML = "武器栏位"
        p.appendChild(label)
        this.m_NumberElement = select = document.createElement("select")
        select.id = `Weapon_${this.m_Id}_number`
        option = document.createElement("option")
        option.textContent = "主武器"
        option.value = "Weapon.PRIMARY"
        select.appendChild(option)
        option = document.createElement("option")
        option.textContent = "副武器"
        option.value = "Weapon.SECONDARY"
        select.appendChild(option)
        option = document.createElement("option")
        option.textContent = "近身武器"
        option.value = "Weapon.MELEE"
        select.appendChild(option)
        select.selectedIndex = 0
        p.appendChild(select)
        this.m_Element.appendChild(p)
        // 切枪延迟
        p = document.createElement('p')
        label = document.createElement("label")
        label.innerHTML = "切枪延迟"
        p.appendChild(label)
        this.m_SwitchDelayElement = input = document.createElement("input")
        input.id = `Weapon_${this.m_Id}_switch_delay`
        input.type = "text"
        p.appendChild(input)
        label = document.createElement("label")
        label.innerHTML = "毫秒"
        p.appendChild(label)
        this.m_Element.appendChild(p)
        // 攻击按键
        p = document.createElement('p')
        label = document.createElement("label")
        label.innerHTML = "攻击按键"
        p.appendChild(label)
        this.m_AttackButtonElements = new Array<HTMLInputElement>
        input = document.createElement("input")
        input.name = `Weapon_${this.m_Id}_attack_button`
        this.m_AttackButtonElements.push(input)
        input.value = "Mouse.LEFT"
        input.type = "radio"
        input.checked = true
        label = document.createElement("label")
        label.innerHTML = "左键"
        p.appendChild(input)
        p.appendChild(label)
        input = document.createElement("input")
        this.m_AttackButtonElements.push(input)
        input.name = `Weapon_${this.m_Id}_attack_button`
        input.value = "Mouse.RIGHT"
        input.type = "radio"
        input.checked = false
        label = document.createElement("label")
        label.innerHTML = "右键"
        p.appendChild(input)
        p.appendChild(label)
        this.m_Element.appendChild(p)
    }
    override gather(): void {
        const parent_gather = Object.getPrototypeOf(ConventionalWeapon.prototype).gather
        parent_gather.call(this) // 调用基类 
        let number = document.getElementById(`Weapon_${this.m_Id}_number`) as HTMLSelectElement
        this.m_Number = number.value
        let switch_delay = document.getElementById(`Weapon_${this.m_Id}_switch_delay`) as HTMLInputElement
        try {
            this.m_SwitchDelay = switch_delay.value
        } catch (e) {
            this.m_SwitchDelay = `100`
        }
        let attack_button = document.querySelector(`input[name='Weapon_${this.m_Id}_attack_button']:checked`) as HTMLInputElement
        this.m_AttackButton = attack_button.value
    }
    override generate(indent: string, indent_level: number = 0, first_line_indent: boolean = true): string {
        return `${first_line_indent ? indent.repeat(indent_level) : ""}Weapon:new{\n` +
        `${indent.repeat(indent_level + 1)}name = "${this.m_Name}",\n` +
        `${indent.repeat(indent_level + 1)}number = ${this.m_Number},\n` +
        `${indent.repeat(indent_level + 1)}purchase_sequence = { ${this.m_PurchaseSequence.join(", ")} },\n` +
        `${indent.repeat(indent_level + 1)}switch_delay = ${this.m_SwitchDelay},\n` +
        `${indent.repeat(indent_level + 1)}attack_button = ${this.m_AttackButton},\n` +
        `${indent.repeat(indent_level)}}`
    }
    override async update(): Promise<void> {
        let base_update = Object.getPrototypeOf(ConventionalWeapon.prototype).update
        base_update.call(this)
        this.m_NumberElement.value = `${this.m_Number}`
        this.m_SwitchDelayElement.value = `${this.m_SwitchDelay}`
        this.m_AttackButtonElements.forEach(e => {
            if (e.value === this.m_AttackButton) {
                e.checked = true
            }
        })
    }
}

export class CustomizedWeapon extends Weapon {
    m_TemplateName: string
    m_TemplateCode: string
    m_TemplateClass: string
    m_TemplateNameElement: HTMLSelectElement
    m_Option: number
    constructor(template_class: string) {
        super()
        this.m_TemplateClass = template_class
        this.m_TemplateName = "无"
        this.m_TemplateCode = ""
        let p = document.createElement('p')
        let label = document.createElement("label")
        label.innerHTML = "武器模板"
        p.appendChild(label)
        let select = document.createElement("select")
        this.m_TemplateNameElement = select
        select.id = `Weapon_${this.m_Id}_template_name`
        let default_option = document.createElement("option")
        default_option.textContent = "无"
        default_option.value = ""
        select.appendChild(default_option)
        for (const weapon of g_WeaponTemplateList[template_class]) {
            let option = document.createElement("option")
            option.textContent = weapon.name
            option.value = "./weapon_templates/" + weapon.file
            select.appendChild(option)
        }

        select.onchange = async (event: Event) => {
            let select = event.target as HTMLSelectElement
            try {
                let code = await CustomizedWeapon.SyncTemplate(select.value)                
                let name = this.m_NameElement
                if (!name.value) { // 用户未指定武器名称时，将其修改为模板名称
                    name.value = select.options[select.selectedIndex].textContent
                }
                this.m_TemplateName = select.options[select.selectedIndex].textContent
                this.m_TemplateCode = code
            } catch (err) {
                console.error(err)
                alert(err.message)
                // 回滚到最近一次正确的选择
                select.selectedIndex = 0 // 先修改为默认选项
                for (let i = 0; i < select.options.length; i++) { // 再匹配出与 m_TemplateName 一致的选项
                    if (select.options[i].textContent === this.m_TemplateName) {
                        select.selectedIndex = i
                        break
                    }
                }
            }
        }
        p.appendChild(select)
        this.m_Element.appendChild(p)
    }
    static async SyncTemplate(url: string): Promise<string> {
        if (!url) {
            return ""
        }
        let response = await fetch(url)
        if (!response.ok) {
            console.log(response)
            throw Error(`获取“${url}”模板文件失败。响应状态：${response.status}`)
        }
        return response.text()
    }
    override generate(indent: string, indent_level: number = 0, first_line_indent: boolean = true): string {
        this.gather()
        let template_ast = luaparse.parse(this.m_TemplateCode)
        let args = new Map<string, any>()
        for (const field of template_ast.body[0].expression.arguments.fields) {
            args.set(LuaASTUtil.GenerateLuaCodeFromAST(field.key), field)
        }
        let ast = luaparse.parse(
            `Weapon:new{\n` +
            `    name = "${this.m_Name}",\n` +
            `    purchase_sequence = { ${this.m_PurchaseSequence.join(", ")} }\n` +
            `}`
        )
        for (let i = 0; i < ast.body[0].expression.arguments.fields.length; i++) {
            const key = LuaASTUtil.GenerateLuaCodeFromAST(ast.body[0].expression.argument.fields[i].key)
            if (args.has(key)) {
                args.set(key, ast.body[0].expression.arguments.fields[i])
            }
        }
        let fields = new Array()
        args.forEach((v) => {
            fields.push(v)
        })
        ast.body[0].expression.arguments.fields = fields
        return LuaASTUtil.GenerateLuaCodeFromAST(ast, indent_level, first_line_indent).replace(/\s+$/, "")
    }
    override async update(): Promise<void> {
        let base_update = Object.getPrototypeOf(ConventionalWeapon.prototype).update
        base_update.call(this)
        this.m_NameElement.value = this.m_Name
        let bValidTemplateName = false
        let select = this.m_TemplateNameElement
        if (!this.m_TemplateName) {
            this.m_TemplateName = "无"
        }
        for (let i = 0; i < select.options.length; i++) {
            if (select.options[i].textContent === this.m_TemplateName) {
                select.selectedIndex = i
                bValidTemplateName = true
                break
            }
        }
        if (!bValidTemplateName) {
            select.selectedIndex = 0
            alert(`无效的模板：${this.m_TemplateName}`)
            this.m_TemplateName = "无"
            // 不修改模板
            return
        }
        if (this.m_TemplateName !== "无") { // 更新模板
            let code = await CustomizedWeapon.SyncTemplate(select.value)
            this.m_TemplateCode = code
        }
    }
}


function ConvertCallExpressionToTableCallExpression(node) {
    if (node.type === LuaASTUtil.AST_NODE_TABLE_CALL_EXPRESSION) {
        return node
    }
    if (
        node.type === LuaASTUtil.AST_NODE_CALL_EXPRESSION &&
        node.arguments.length === 1 &&
        node.arguments[0].type === LuaASTUtil.AST_NODE_TABLE_CONSTRUCTOR_EXPRESSION
    ) {
        return {
            type: LuaASTUtil.AST_NODE_TABLE_CALL_EXPRESSION,
            base: node.base,
            argument: node.arguments[0],
            arguments: node.arguments[0]
        }
    } else {
        throw Error(`Failed to convert node to AST_NODE_TABLE_CALL_EXPRESSION:  illegal node type.\nNode:${node}`)
    }
}


function __resolve_weapon(ast) {
    let fields = new Map()
    if (ast.type !== LuaASTUtil.AST_NODE_TABLE_CALL_EXPRESSION) {
        return undefined
    }
    if (LuaASTUtil.GenerateLuaCodeFromAST(ast.base) !== "Weapon:new") {
        return undefined
    }
    let args = ast.arguments
    let string_pattern = /^(['"])(.*?)\1$/
    for (const field of args.fields) {
        if (LuaASTUtil.GenerateLuaCodeFromAST(field.key) === "name") {
            fields.set("name", LuaASTUtil.GenerateLuaCodeFromAST(field.value).replace(string_pattern, "$2"))
        } else if (LuaASTUtil.GenerateLuaCodeFromAST(field.key) === "purchase_sequence") {
            let purchase_sequence = new Array()
            for (const f of field.value.fields) {
                purchase_sequence.push(LuaASTUtil.GenerateLuaCodeFromAST(f.value))
            }
            fields.set("purchase_sequence", purchase_sequence)
        } else if (LuaASTUtil.GenerateLuaCodeFromAST(field.key) === "template_name") {
            fields.set("template_name", LuaASTUtil.GenerateLuaCodeFromAST(field.value).replace(string_pattern, "$2"))
        } else if (LuaASTUtil.GenerateLuaCodeFromAST(field.key) === "number") {
            fields.set("number", LuaASTUtil.GenerateLuaCodeFromAST(field.value))
        } else if (LuaASTUtil.GenerateLuaCodeFromAST(field.key) === "switch_delay") {
            fields.set("switch_delay", LuaASTUtil.GenerateLuaCodeFromAST(field.value))
        } else if (LuaASTUtil.GenerateLuaCodeFromAST(field.key) === "attack_button") {
            fields.set("attack_button", LuaASTUtil.GenerateLuaCodeFromAST(field.value))
        } else if (LuaASTUtil.GenerateLuaCodeFromAST(field.key) === "attack") {
            fields.set("template_code", LuaASTUtil.GenerateLuaCodeFromAST(ast))
        } else if (LuaASTUtil.GenerateLuaCodeFromAST(field.key) === "use") {
            fields.set("template_code", LuaASTUtil.GenerateLuaCodeFromAST(ast))
        }
    }
    return fields
}

export function ResolveWeaponList(ast) {
    let armor: Armor
    var default_part_weapons = new Array<PartWeapon>()
    var extended_part_weapons = new Array<PartWeapon>()
    var default_conventional_weapons = new Array<ConventionalWeapon | CustomizedWeapon>()
    var extended_conventional_weapons = new Array<ConventionalWeapon | CustomizedWeapon>()
    var default_special_weapons = new Array<CustomizedWeapon>()
    var extended_special_weapons = new Array<CustomizedWeapon>()

    let queue = new Array()
    ast.body.forEach(e => {
        queue.push(e)
    })
    while (queue.length > 0) {
        let node = queue.shift()
        if (node.type === LuaASTUtil.AST_NODE_ASSIGNMENT_STATEMENT) {
            let index = 0
            while (index < node.variables.length) {
                let variable = LuaASTUtil.GenerateLuaCodeFromAST(node.variables[index], 0)
                if (variable === "Armor" || variable === "AC") {
                    let dict = __resolve_weapon(ConvertCallExpressionToTableCallExpression(node.init[index]))
                    if (!dict) continue
                    armor = new Armor()
                    armor.m_Name = dict.get("name")
                    armor.m_PurchaseSequence = dict.get("purchase_sequence")
                } else if (variable === "DefaultPartWeapons" || variable === "PartWeaponList") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        let dict = __resolve_weapon(ConvertCallExpressionToTableCallExpression(weapons[i].value))
                        if (!dict) continue
                        let weapon = new PartWeapon()
                        weapon.m_Name = dict.get("name")
                        weapon.m_PurchaseSequence = dict.get("purchase_sequence")
                        default_part_weapons.push(weapon)
                    }
                } else if (variable === "DefaultConventionalWeapons" || variable === "DefaultWeaponList") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        let dict = __resolve_weapon(ConvertCallExpressionToTableCallExpression(weapons[i].value))
                        if (!dict) continue
                        if (dict.has("template_name") || dict.has("template_code")) {
                            let weapon = new CustomizedWeapon("常规")
                            weapon.m_Name = dict.get("name")
                            weapon.m_PurchaseSequence = dict.get("purchase_sequence")
                            weapon.m_TemplateName = dict.get("template_name")
                            weapon.m_TemplateCode = dict.get("template_code")
                            default_conventional_weapons.push(weapon)
                        } else {
                            let weapon = new ConventionalWeapon()
                            weapon.m_Name = dict.get("name")
                            weapon.m_PurchaseSequence = dict.get("purchase_sequence")
                            weapon.m_Number = dict.get("number")
                            weapon.m_AttackButton = dict.get("attack_button")
                            weapon.m_SwitchDelay = dict.get("switch_delay")
                            default_conventional_weapons.push(weapon)
                        }
                    }
                } else if (variable === "DefaultSpecialWeapons") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        let dict = __resolve_weapon(ConvertCallExpressionToTableCallExpression(weapons[i].value))
                        let weapon = new CustomizedWeapon("特殊")
                        weapon.m_Name = dict.get("name")
                        weapon.m_PurchaseSequence = dict.get("purchase_sequence")
                        weapon.m_TemplateName = dict.get("template_name")
                        weapon.m_TemplateCode = dict.get("template_code")
                        default_special_weapons.push(weapon)
                    }
                } else if (variable === "ExtendedPartWeapons") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        let dict = __resolve_weapon(ConvertCallExpressionToTableCallExpression(weapons[i].value))
                        if (!dict) continue
                        let weapon = new PartWeapon()
                        weapon.m_Name = dict.get("name")
                        weapon.m_PurchaseSequence = dict.get("purchase_sequence")
                        extended_part_weapons.push(weapon)
                    }
                } else if (variable === "ExtendedConventionalWeapons" || variable === "ExtendedWeaponList") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        let dict = __resolve_weapon(ConvertCallExpressionToTableCallExpression(weapons[i].value))
                        if (!dict) continue
                        if (dict.has("template_name") || dict.has("template_code")) {
                            let weapon = new CustomizedWeapon("常规")
                            weapon.m_Name = dict.get("name")
                            weapon.m_PurchaseSequence = dict.get("purchase_sequence")
                            weapon.m_TemplateName = dict.get("template_name")
                            weapon.m_TemplateCode = dict.get("template_code")
                            extended_conventional_weapons.push(weapon)
                        } else {
                            let weapon = new ConventionalWeapon()
                            weapon.m_Name = dict.get("name")
                            weapon.m_PurchaseSequence = dict.get("purchase_sequence")
                            weapon.m_Number = dict.get("number")
                            weapon.m_AttackButton = dict.get("attack_button")
                            weapon.m_SwitchDelay = dict.get("switch_delay")
                            extended_conventional_weapons.push(weapon)
                        }
                    }
                } else if (variable === "ExtendedSpecialWeapons") {
                    let weapons = node.init[index].fields
                    for (let i = 0; i < weapons.length; i++) {
                        let dict = __resolve_weapon(ConvertCallExpressionToTableCallExpression(weapons[i].value))
                        let weapon = new CustomizedWeapon("特殊")
                        weapon.m_Name = dict.get("name")
                        weapon.m_PurchaseSequence = dict.get("purchase_sequence")
                        weapon.m_TemplateName = dict.get("template_name")
                        weapon.m_TemplateCode = dict.get("template_code")
                        extended_special_weapons.push(weapon)
                    }
                } else if (variable === "SpecialWeapon") {
                    let dict = __resolve_weapon(ConvertCallExpressionToTableCallExpression(node.init[index]))
                    let weapon = new CustomizedWeapon("特殊")
                    weapon.m_Name = dict.get("name")
                    weapon.m_PurchaseSequence = dict.get("purchase_sequence")
                    weapon.m_TemplateName = dict.get("template_name")
                    weapon.m_TemplateCode = dict.get("template_code")
                    extended_special_weapons.push(weapon)
                }
                index++
            }
        }
        for (const property in node) {
            let subnode = node[property] as Object
            if (Array.isArray(subnode)) {
                subnode.forEach(e => {
                    if (e.hasOwnProperty("type")) {
                        queue.push(e)
                    }
                })
            } else if (subnode?.hasOwnProperty("type")) {
                queue.push(subnode)
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