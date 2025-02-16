import * as LuaASTUtil from "./LuaASTUtil.js"
/**
 * 开关事件发布者
 */
class SwitchEventPublisher {
    private static s_Publishers = new Map<string, SwitchEventPublisher>
    m_Id: string
    m_State: boolean
    m_Subscribers: string[] /* 订阅者 Id */
    constructor(id: string, state: boolean) {
        this.m_Id = id
        this.m_State = state
        this.m_Subscribers = []
        SwitchEventPublisher.s_Publishers.set(id, this)
    }
    public static GetPublisherById(id: string): SwitchEventPublisher | undefined {
        return SwitchEventPublisher.s_Publishers.get(id)
    }
    public set() {
        this.m_State = true
        this.broadcast()
    }
    public reset() {
        this.m_State = false
        this.broadcast()
    }
    public register(subscriber_id: string) {
        this.m_Subscribers.push(subscriber_id) // 加入订阅者列表
    }
    public static bootstrap() {
        SwitchEventPublisher.s_Publishers.forEach((v, _) => {
            v.broadcast()
        });
    }
    private broadcast() {
        for (const id of this.m_Subscribers) {
            SwitchEventSubscriber.GetSubscriberById(id)?.notify(this.m_Id, this.m_State)
        }
    }
}
/**
 * 开关事件订阅者
 */
class SwitchEventSubscriber {
    static s_Subscribers = new Map<string, SwitchEventSubscriber>()
    m_Id: string
    m_Publishers: Map<string, boolean>
    constructor(id: string) {
        this.m_Id = id
        this.m_Publishers = new Map<string, boolean>()
        SwitchEventSubscriber.s_Subscribers.set(id, this)
    }
    public static GetSubscriberById(id: string): SwitchEventSubscriber | undefined {
        return SwitchEventSubscriber.s_Subscribers.get(id)
    }
    public subscribe(publisher_id: string) {
        /* 下面两条语句次序不可调换，因为 notify() 不会接收不在发布者列表中的发布者提供的内容 */
        this.m_Publishers.set(publisher_id, false) // 加入发布者列表
        SwitchEventPublisher.GetPublisherById(publisher_id)?.register(this.m_Id) // 让发布者为自己完成注册
    }
    public notify(publisher_id: string, state: boolean) {
        if (this.m_Publishers.has(publisher_id)) {
            this.m_Publishers.set(publisher_id, state)
        }
        // 此处不能用 forEach，return 要在当前方法内返回
        for (const [_, v] of this.m_Publishers) {
            if (!v) {
                document.getElementById(this.m_Id)?.
                    setAttribute("style", "display: none")
                return
            }
        }
        document.getElementById(this.m_Id)?.
            setAttribute("style", "display: block")
    }
    /**
     * 检查该订阅者是否处于激活状态（其订阅的所有事件是否均已经激活）
     * @returns {boolean}
     */
    public activated(): boolean {
        for (const [_, signaled] of this.m_Publishers) {
            if (!signaled) {
                return false
            }
        }
        return true;
    }
}

function WidgetFactory(widget_def: any, level: number): Widget | undefined {
    if (!widget_def) {
        return undefined
    }
    widget_def.level = level
    if (widget_def.type === "SECTION") {
        let components = new Array<Widget>()
        widget_def.components?.map(_c => {
            let widget = WidgetFactory(_c, level + 1)
            if (widget) {
                components.push(widget)
            }
        })
        return new SectionWidget({ ...widget_def, components: components })
    } else if (widget_def.type === "FIELD") {
        return new FieldWidget(widget_def)
    } else if (widget_def.type == "POSITION") {
        return new PositionWidget(widget_def)
    } else if (widget_def.type === "SWITCH") {
        return new SwitchWidget(widget_def)
    } else if (widget_def.type === "KEYSTROKES") {
        return new KeystrokesWidget(widget_def)
    }
}


class Widget {
    private static s_HoverImage: HTMLImageElement // 描述图片
    static {
        Widget.s_HoverImage = document.createElement("img") as HTMLImageElement
        Widget.s_HoverImage.id = "PopUmImage"
        Widget.s_HoverImage.classList.add("PopUpImage")
        Widget.s_HoverImage.setAttribute("style", "display: none")
        document.body.appendChild(Widget.s_HoverImage)
    }
    id: string // 组件标识符
    type: string // 组件类型
    title: string // 本节标题
    level: number // 本节层级，对应于标题层级
    depends_on: string[] | undefined // 依赖的开关组件列表，保依赖的组件 id，用于控制该组件是否启用
    legacy_name: string | undefined // 该字段的旧名，用于保持解析旧版配置内容时向下兼容
    annotation: string | undefined // 组件批注，用于补充说明
    image: string | undefined // 描述图片
    element: HTMLDivElement // 组件对应的 HTML 对象
    switch_event_subscriber: SwitchEventSubscriber | undefined // 如果存在依赖，则此字段有定义
    constructor(data: { id: string, type: string, level: number, title: string } & Partial<Widget>) {
        this.id = data.id
        this.type = data.type
        this.level = data.level
        this.title = data.title
        this.depends_on = data.depends_on
        this.legacy_name = data.legacy_name
        this.annotation = data.annotation
        this.image = data.image
        // 创建顶层容器
        this.element = document.createElement("div") as HTMLDivElement
        this.element.id = this.id
        // 放置标题
        let title = document.createElement(`h${this.level}`) as HTMLHeadingElement
        title.innerHTML = this.title
        this.element.appendChild(title)
        // 如有批注，则在容器中放置批注
        if (this.annotation) {
            let annotation = document.createElement("p")
            annotation.innerHTML = this.annotation
            this.element.appendChild(annotation)
        }
        if (this.image) {
            title.setAttribute("style", "color: blue;")
            title.onmouseover = () => {
                Widget.s_HoverImage.src = this.image || ""
                Widget.s_HoverImage.setAttribute("style", "display: block;")
            }
            title.onmouseleave = () => {
                Widget.s_HoverImage.src = ""
                Widget.s_HoverImage.setAttribute("style", "display: none;")
            }
        }
        // 注册依赖
        if (this.depends_on) {
            this.switch_event_subscriber = new SwitchEventSubscriber(this.id)
            this.depends_on.map(publisher_container_id => {
                this.switch_event_subscriber?.subscribe(publisher_container_id)
            })
        }
    }
    collect(settings: Map<string, string>) { }
    gather(settings: Map<string, string>) { }
}

class SectionWidget extends Widget {
    components: Widget[] | undefined
    constructor(
        data: { id: string, type: string, level: number, title: string, components: Widget[] | undefined } & Partial<SectionWidget>
    ) {
        super(data)
        this.components = data.components
        this.components?.map(_component => { // 将组件 HTML 元素添挂载到本层元素节点
            this.element.appendChild(_component.element)
        })
    }
    override collect(settings: Map<string, string>): void {
        this.components?.map(_c => {
            _c.collect(settings)
        })
    }
    override gather(set: Map<string, string>): void {
        this.components?.map(_c => {
            _c.gather(set)
        })
    }
}

class FieldWidget extends Widget {
    pre_label: string
    post_label: string
    value: string
    pattern: RegExp
    quoted: boolean
    field_id: string
    field_element: HTMLInputElement
    constructor(data: {
        id: string,
        type: string,
        level: number,
        title: string
    }
        & Partial<FieldWidget>) {
        super(data)
        this.field_id = `FIELD_${this.id}`
        this.pre_label = data.pre_label ?? ""
        this.value = data.value ?? ""
        this.post_label = data.post_label ?? ""
        this.pattern = data.pattern ?? /^.*$/
        this.quoted = data.quoted ? true : false
        let p = document.createElement('p')

        let pre_label = document.createElement("label")
        pre_label.innerHTML = this.pre_label
        p.appendChild(pre_label)

        this.field_element = document.createElement("input")
        this.field_element.type = "text"
        this.field_element.id = this.field_id
        this.field_element.value = data.value ?? ""
        p.appendChild(this.field_element)

        let post_label = document.createElement("label")
        post_label.innerHTML = this.post_label
        p.appendChild(post_label)

        this.element.appendChild(p)
    }
    override gather(set: Map<string, string>): void {
        let match = this.pattern.exec(this.value)
        let result: string
        let _field = this.field_element.value.trim()
        if (!_field) {
            if (!this.switch_event_subscriber || this.switch_event_subscriber.activated()) {
                throw this
            }
        } else {
            if (this.quoted) {
                set.set(this.field_id, `"${_field}"`)
            } else {
                set.set(this.field_id, _field)
            }
        }
    }
    override collect(settings: Map<string, string>): void {
        function remove_quotes(s: string): string {
            return s.replace(/^(['"])(.*?)\1$/, "$2")
        }
        settings.forEach((v, k) => {
            if (k === this.field_id || k === this.legacy_name) {
                this.field_element.value = this.quoted ? remove_quotes(v) : v
            }
        })
    }
}

class PositionWidget extends Widget {
    pre_labels: string[] | undefined
    post_labels: string[] | undefined
    values: string[] | undefined
    x_field_id: string
    y_field_id: string
    x_field_element: HTMLInputElement
    y_field_element: HTMLInputElement
    constructor(data: {
        id: string,
        type: string,
        level: number,
        title: string
    } & Partial<PositionWidget>) {
        super(data)

        this.x_field_id = `POSITION_${this.id}_X`
        this.y_field_id = `POSITION_${this.id}_Y`

        this.pre_labels = data.pre_labels ? (data.pre_labels.length >= 2 ? data.pre_labels : [data.pre_labels[0], ""]) : ["", ""]
        this.post_labels = data.post_labels ? (data.post_labels.length >= 2 ? data.post_labels : [data.post_labels[0], ""]) : ["", ""]
        this.values = data.values ? (data.values.length >= 2 ? data.values : [data.values[0], ""]) : ["", ""]

        let x = document.createElement('p')
        let x_prelabel = document.createElement("label")
        x_prelabel.innerHTML = this.pre_labels[0]
        x.appendChild(x_prelabel)
        this.x_field_element = document.createElement("input")
        this.x_field_element.id = this.x_field_id
        this.x_field_element.type = "text"
        this.x_field_element.value = this.values[0]
        x.appendChild(this.x_field_element)
        let x_postlabel = document.createElement("label")
        x_postlabel.innerHTML = this.post_labels[0]
        x.appendChild(x_postlabel)
        this.element.appendChild(x)

        let y = document.createElement('p')
        let y_prelabel = document.createElement("label")
        y_prelabel.innerHTML = this.pre_labels[1]
        y.appendChild(y_prelabel)
        this.y_field_element = document.createElement("input")
        this.y_field_element.id = this.y_field_id
        this.y_field_element.type = "text"
        this.y_field_element.value = this.values[1]
        y.appendChild(this.y_field_element)
        let y_postlabel = document.createElement("label")
        y_postlabel.innerHTML = this.post_labels[1]
        y.appendChild(y_postlabel)
        this.element.appendChild(y)
    }
    override gather(setting: Map<string, string>): void {
        let _x = this.x_field_element.value.trim()
        let _y = this.y_field_element.value.trim()
        if (!_x || !_y) {
            if (!this.switch_event_subscriber || this.switch_event_subscriber.activated()) {
                throw this
            }
        } else {
            setting.set(this.x_field_id, _x)
            setting.set(this.y_field_id, _y)
        }
    }

    override collect(settings: Map<string, string>): void {
        if (settings.has(this.x_field_id)) {
            this.x_field_element.value = settings.get(this.x_field_id)
        } else if (settings.has(`${this.legacy_name}_X`)) {
            this.x_field_element.value = settings.get(`${this.legacy_name}_X`)
        }
        if (settings.has(this.y_field_id)) {
            this.y_field_element.value = settings.get(this.y_field_id)
        } else if (settings.has(`${this.legacy_name}_Y`)) {
            this.y_field_element.value = settings.get(`${this.legacy_name}_Y`)
        }
    }
}

class SwitchWidget extends Widget {
    labels: string[]
    values: string[]
    true_option: number
    checked: number | undefined
    switch_event_publisher: SwitchEventPublisher
    switch_name: string
    switch_elements: HTMLInputElement[]
    constructor(data: {
        id: string,
        type: string,
        level: number,
        title: string
    } & Partial<SwitchWidget>) {
        super(data)
        this.switch_name = `SWITCH_${this.id}`
        this.labels = new Array<string>()
        this.labels = data.labels ? (data.labels.length >= 2 ? data.labels : [data.labels[0], ""]) : ["", ""]
        this.values = data.values ? (data.values.length >= 2 ? data.values : [data.values[0], ""]) : ["true", "false"]
        this.true_option = data.true_option ?? 0
        this.true_option = (this.true_option === 0 || this.true_option === 1) ? this.true_option : 0
        this.checked = data.checked
        this.switch_event_publisher = new SwitchEventPublisher(this.id, this.checked === this.true_option)
        this.switch_elements = new Array<HTMLInputElement>()
        for (let i = 0; i < 2; i++) {
            let p = document.createElement('p') as HTMLParagraphElement
            let radio = document.createElement("input") as HTMLInputElement
            radio.name = this.switch_name
            radio.type = "radio"
            radio.value = this.values[i]
            if (i === this.checked) {
                radio.checked = true
            }
            radio.onclick = () => {
                if (i === this.true_option) {
                    this.switch_event_publisher.set()
                } else {
                    this.switch_event_publisher.reset()
                }
            }
            this.switch_elements[i] = radio
            p.appendChild(radio)
            let radio_label = document.createElement("label") as HTMLLabelElement
            radio_label.innerHTML = this.labels[i]
            p.appendChild(radio_label)
            this.element.appendChild(p)
        }
    }
    override gather(set: Map<string, string>): void {
        let switches = document.querySelectorAll(`input[name=SWITCH_${this.id}]`) as NodeListOf<HTMLInputElement>
        set.set(`SWITCH_${this.id}`, "nil")
        if (!switches) {
            return
        }
        for (const s of switches) {
            if (s.checked) {
                set.set(`SWITCH_${this.id}`, s.value)
            }
        }
    }
    override collect(settings: Map<string, string>): void {
        settings.forEach((v, k) => {
            if (k === this.switch_name || k === this.legacy_name) {
                this.switch_elements.forEach(s => {
                    if (s.value == v) {
                        s.click()
                    }
                })
            }
        });
    }
}

class KeystrokesWidget extends Widget {
    label: string
    keystroke_element: HTMLInputElement
    record_button_element: HTMLButtonElement
    values: Array<string>
    static s_KEYMAP: Map<string, string> // A: Keyboard.A
    static s_REVERSE_KEYMAP: Map<string, string> // Keyboard.A: A
    static s_Recorder: KeystrokesWidget | undefined = undefined
    static {
        KeystrokesWidget.s_KEYMAP = new Map<string, string>()
        KeystrokesWidget.s_REVERSE_KEYMAP = new Map<string, string>()
        KeystrokesWidget.s_KEYMAP.set('0', "Keyboard.ZERO")
        KeystrokesWidget.s_KEYMAP.set('1', "Keyboard.ONE")
        KeystrokesWidget.s_KEYMAP.set('2', "Keyboard.TWO")
        KeystrokesWidget.s_KEYMAP.set('3', "Keyboard.THREE")
        KeystrokesWidget.s_KEYMAP.set('4', "Keyboard.FOUR")
        KeystrokesWidget.s_KEYMAP.set('5', "Keyboard.FIVE")
        KeystrokesWidget.s_KEYMAP.set('6', "Keyboard.SIX")
        KeystrokesWidget.s_KEYMAP.set('7', "Keyboard.SEVEN")
        KeystrokesWidget.s_KEYMAP.set('8', "Keyboard.EIGHT")
        KeystrokesWidget.s_KEYMAP.set('9', "Keyboard.NINE")
        let A = 'A'.charCodeAt(0)
        let Z = 'Z'.charCodeAt(0)
        for (let i = A; i <= Z; i++) {
            let c = String.fromCharCode(i)
            KeystrokesWidget.s_KEYMAP.set(c, `Keyboard.${c}`)
        }
        KeystrokesWidget.s_KEYMAP.forEach((v, k) => {
            this.s_REVERSE_KEYMAP.set(v, k)
        })
    }
    static s_Keystrokes: Array<string>
    static OnKeyUp(e: KeyboardEvent) {
        let key = e.key.toUpperCase()
        if (KeystrokesWidget.s_KEYMAP.has(key)) {
            KeystrokesWidget.s_Keystrokes.push(key)
            KeystrokesWidget.s_Recorder.keystroke_element.value = KeystrokesWidget.s_Keystrokes.join(' ')
        }
    }
    start_recording() {
        if (KeystrokesWidget.s_Recorder) {
            KeystrokesWidget.s_Recorder.stop_recording()
        }
        KeystrokesWidget.s_Recorder = this
        this.keystroke_element.value = ""
        KeystrokesWidget.s_Keystrokes = new Array<string>()
        this.record_button_element.textContent = "⏹️"
        this.record_button_element.onclick = () => {
            this.stop_recording()
        }
        KeystrokesWidget.s_Keystrokes = new Array<string>()
        document.addEventListener("keyup", KeystrokesWidget.OnKeyUp)
    }
    stop_recording() {
        if (KeystrokesWidget.s_Recorder !== this) {
            return
        }
        document.removeEventListener("keyup", KeystrokesWidget.OnKeyUp)
        this.values = new Array<string>
        KeystrokesWidget.s_Keystrokes.forEach(e => {
            let _e = KeystrokesWidget.s_KEYMAP.get(e)
            if (_e) {
                this.values.push(_e)
            }
        })
        KeystrokesWidget.s_Recorder = undefined
        this.record_button_element.textContent = "⏺️"
        this.record_button_element.onclick = () => {
            this.start_recording()
        }
    }
    constructor(data: {
        id: string,
        type: string,
        level: number,
        title: string
    } & Partial<KeystrokesWidget>) {
        super(data)
        this.values = data.values || []
        let p = document.createElement('p')
        this.label = data.label || ""
        let label = document.createElement("label")
        label.innerHTML = this.label
        p.appendChild(label)
        let input = document.createElement("input")
        input.id = `KEYSTROKES_${this.id}`
        this.keystroke_element = input
        input.type = "text"
        input.readOnly = true
        input.value = this.values.map(e => { return KeystrokesWidget.s_REVERSE_KEYMAP.get(e) }).join(' ')
        p.appendChild(input)
        let button = document.createElement("button")
        this.record_button_element = button
        button.textContent = "⏺️"
        button.onclick = () => {
            this.start_recording()
        }
        p.appendChild(button)
        this.element.appendChild(p)
    }
    override gather(settings: Map<string, string>): void {
        if (KeystrokesWidget.s_Recorder) {
            KeystrokesWidget.s_Recorder.stop_recording()
        }
        if (!this.values || this.values.length === 0) {
            throw this
        }
        settings.set(`KEYSTROKES_${this.id}`, `{ ${this.values.join(", ")} }`)
    }
    override collect(settings: Map<string, string>): void {
        let item = settings.get(`KEYSTROKES_${this.id}`)
        if (item) {
            let keystrokes = item.replace(/\s/g, "").replace(/^\{(.*)\}$/g, "$1").split(',')
            this.values = new Array<string>()
            this.keystroke_element.value = ""
            keystrokes.forEach(e => {
                if (KeystrokesWidget.s_REVERSE_KEYMAP.has(e)) {
                    this.values.push(e)
                    this.keystroke_element.value += KeystrokesWidget.s_REVERSE_KEYMAP.get(e) + ' '
                }
            })
            this.keystroke_element.value = this.keystroke_element.value.trim()
        }
    }
}

/**
 * 从 Lua 抽象语法树中导入设置，并将设置填入配置面板。
 * @param ast Lua 抽象语法树
 * @returns 从抽象语法树中解析得到的键值对形式的设置
 */
function ImportSetting(ast: any): Map<string, string> {
    // 广度优先遍历找到 Setting 的定义
    let settings = new Map<string, string>()
    if (!ast) {
        return settings
    }
    let settings_node
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
                if (variable === "Setting") {
                    settings_node = node.init[index]
                    break
                }
                index++
            }
        }
        // 采取非递归广度优先遍历，同级节点入队
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
    if (settings_node.type !== LuaASTUtil.AST_NODE_TABLE_CONSTRUCTOR_EXPRESSION) {
        throw `Assertion Failed: settings_node.type !== AST_NODE_TABLE_CONSTRUCTOR_EXPRESSION`
    }
    settings_node.fields.forEach(e => {
        LuaASTUtil.SetIndentUnit('')
        settings.set(
            LuaASTUtil.GenerateLuaCodeFromAST(e.key, 0),
            LuaASTUtil.GenerateLuaCodeFromAST(e.value, 0)
        )
        LuaASTUtil.SetIndentUnit('\t')
    })
    console.log(settings)
    return settings
}

/**
 * 从面板中以 Lua 源码形式导出设置
 * @param root 配置面板根组件
 * @returns 包含设置的 Lua 代码块
 */
function ExportSetting(root: Widget): string {
    let setting = new Map<string, string>()
    try {
        root.gather(setting)
        console.log(setting)
        let snippet = "if not Setting_lua\n" +
            "then\n" +
            "\tSetting_lua = true\n" +
            "\tSetting = {\n"
        setting.forEach((v, k) => {
            snippet += `\t\t${k} = ${v},\n`
        })
        snippet += "\t}\n"
        snippet += "end\n"
        return snippet
    } catch (widget) {
        alert(`${widget.title}`)
        widget.element.scrollIntoView({ behavior: 'smooth', block: 'center' })
        widget.element.classList.add('Glimmer')
        setTimeout(() => {
            widget.element.classList.remove("Glimmer")
        }, 1000)
        return ""
    }
}

export {
    SwitchEventPublisher,
    SwitchEventSubscriber,
    Widget,
    SectionWidget,
    FieldWidget,
    PositionWidget,
    SwitchWidget,
    WidgetFactory,
    ImportSetting,
    ExportSetting
}