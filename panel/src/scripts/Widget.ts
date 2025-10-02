export type WidgetType = "NULL"|"SECTION"|"FIELD"|"POSITION"|"SWITCH"|"KEYSTROKES";

export type BaseWidget_T = {
    id: string
    type: WidgetType
    title: string
    image?: string
    annotation?: string
    level?: 1|2|3|4|5|6
    depends_on?: { key: string, value: string }[]
    legacy_name?: string
    ignore?: boolean
};

export type SectionWidget_T = BaseWidget_T & {
    children?: BaseWidget_T[]
};

export type FieldWidget_T = BaseWidget_T & {
    label: string
    hint?: string
    quoted?: boolean
    value?: string | undefined
};

export type SwitchWidget_T = BaseWidget_T & {
    label: string
    options: { text: string, value: string }[]
    value?: string,
};

export type PositionWidget_T = BaseWidget_T & {
    x: { label: string, value?: string }
    y: { label: string, value?: string }
};
export type KeystrokesWidget_T = BaseWidget_T & {
    label: string
    value?: string
    hint: string
};
