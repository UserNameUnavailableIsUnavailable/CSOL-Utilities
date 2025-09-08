#Requires AutoHotkey v2.0

#SingleInstance Force
#Include "Windows.ahk"

if not (A_IsAdmin)
{
    try
    {
        if (A_IsCompiled)
            ShellExecute(0, "runas", A_ScriptFullPath, "/restart", 0, SW_SHOWNORMAL)
        else
            ShellExecute(0, "runas", A_AhkPath, '/restart "' A_ScriptFullPath '"', 0, SW_SHOWNORMAL)
    }
    ExitApp
}

buf := Buffer(512)

DllCall("GetCurrentDirectoryW", "UInt", 512, "Ptr", buf.Ptr)

OnExit OnExitClearUp

global hModule := DllCall("LoadLibraryW", "WStr", "Tool.dll", "Ptr")

if (not hModule)
{
    str := StrGet(buf, "UTF-16")

    MsgBox "加载 Tool.dll 发生错误。错误代码：" A_LastError, "Tool"
    ExitApp
}
if (!DllCall("Tool.dll\InitializeToolDll"))
{
    MsgBox "初始化 Tool 运行环境发生错误。错误代码：" A_LastError, "Tool"
    ExitApp
}

global gameModeToggle := false

#Down::
{
    DllCall(
        "Tool.dll\MinimizeForegroundWindow",
    )
}

#Up::
{
    DllCall(
        "Tool.dll\RestoreMinimizedWindow",
    )
}

AppsKey::
{
    DllCall(
        "Tool.dll\ChangeForegroundWindowInputLanguage",
    )
}

#+T::
{
    DllCall(
        "Tool.dll\ToggleTray",
    )
}

#T::
{
    DllCall("Tool.dll\ToggleTopmostForegroundWindow")
}

#UseHook true
^!h::
{
    global gameModeToggle := !gameModeToggle
    DllCall("Tool.dll\ToggleCursorClipper")
}
#UseHook false

^!C::
{
    hForegroundWindow := GetForegroundWindow()
    DllCall("Tool.dll\CenterWindowClientArea", "Ptr", hForegroundWindow)
}

#HotIf gameModeToggle
<!Tab::LAlt
<!Esc::LAlt
~LWin:: Send "{Blind}{vk07}"
<#Tab:: return
`:: return
A_MenuMaskKey := "vkFF"
#Hotif

OnExitClearUp(*)
{
    DllCall("Tool.dll\DeinitializeToolDll")
    DllCall("FreeLibrary", "Ptr", hModule)
}
