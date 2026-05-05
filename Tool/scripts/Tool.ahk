#Requires AutoHotkey v2.0
#SingleInstance Force

;@Ahk2Exe-ExeName Tool.exe
;@Ahk2Exe-SetMainIcon Tool.ico
;@Ahk2Exe-AddResource Tool.exe.manifest

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

OnExit OnExitCleanUp

global hModule := DllCall("LoadLibraryW", "WStr", "Tool.dll", "Ptr")

if (not hModule)
{
    str := StrGet(buf, "UTF-16")

    MsgBox "Failed to load Tool.dll. Error code：" A_LastError, "Tool"
    ExitApp
}

DllCall("Tool.dll\Setup")

global gameModeToggle := false

; #Down::
; {
;     DllCall(
;         "Tool.dll\MinimizeForegroundWindow",
;     )
; }
;
; #Up::
; {
;     DllCall(
;         "Tool.dll\RestoreMinimizedWindow",
;     )
; }

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
    hWnd := DllCall("GetForegroundWindow", "Ptr")
    DllCall("Tool.dll\ToggleTopmostWindow", "Ptr", hWnd)
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
    DllCall("Tool.dll\CenterClient", "Ptr", hForegroundWindow)
}

#HotIf gameModeToggle
<!Tab::LAlt
<!Esc::LAlt
~LWin:: Send "{Blind}{vk07}"
<#Tab:: return
`:: return
A_MenuMaskKey := "vkFF"
#Hotif

OnExitCleanUp(*)
{
    DllCall("Tool.dll\Cleanup")
    DllCall("FreeLibrary", "Ptr", hModule)
}
