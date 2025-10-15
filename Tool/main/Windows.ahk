#Requires AutoHotkey v2.0
#SingleInstance Force

global NULL := 0
global SW_HIDE := 0
global SW_SHOWNORMAL := 1
global SW_MINIMIZE := 6
global SW_RESTORE := 9
global SW_SHOWDEFAULT := 10

global WM_INPUTLANGCHANGEREQUEST  := 0x0050
global GWL_EXSTYLE := -20

global HWND_NOTOPMOST := -2
global HWND_TOPMOST := -1
global HWND_TOP := 0
global HWND_BOTTOM := 1
global WS_EX_TOPMOST := 0x00000008

global SWP_NOSIZE := 0x0001
global SWP_NOMOVE := 0x0002

global CREATE_SUSPENDED := 0x00000004

ShellExecute(hwnd, lpOperation, lpFile, lpParam, lpDirectory, nShowCmd)
{
    return DllCall(
        "Shell32\ShellExecute",
        "Ptr", hwnd,
        "Str", lpOperation,
        "Str", lpFile,
        "Str", lpParam,
        "Str", lpDirectory,
        "Int", nShowCmd
    )
}

SendMessage(hWnd, msg, wParam, lParam)
{
    return DllCall(
        "SendMessage",
        "Ptr", hWnd,
        "UInt", msg,
        "Int", wParam,
        "Int", lParam
    )
}

GetForegroundWindow(*)
{
    return DllCall("GetForegroundWindow")
}

ShowWindow(hWnd, nCmdShow)
{
    return DllCall(
        "ShowWindow",
        "Ptr", hWnd,
        "Int", nCmdShow
    )
}

FindWindow(lpClassName, lpWindowName)
{
    return DllCall(
        "FindWindow",
        "Str", lpClassName,
        "Str", lpWindowName
    )
}

IsWindowVisible(hWnd)
{
    return DllCall(
        "IsWindowVisible",
        "Ptr", hWnd
    )
}

GetWindowLong(hWnd, nIndex)
{
    return DllCall(
        "GetWindowLong",
        "Ptr", hWnd,
        "Int", nIndex
    )
}

SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags)
{
    return DllCall(
        "SetWindowPos",
        "Ptr", hWnd,
        "Ptr", hWndInsertAfter,
        "Int", X,
        "Int", Y,
        "Int", cx,
        "Int", cy,
        "UInt", uFlags
    )
}

CreateThread(
    lpThreadAttributes, 
    dwStackSize, 
    lpStartAddress, 
    lpParameter, 
    dwCreationFlags, 
    lpThreadId
) {
    return DllCall(
        "CreateThread",
        "Ptr", lpThreadAttributes,
        "UInt64", dwCreationFlags,
        "Ptr", lpStartAddress,
        "Ptr", lpParameter,
        "UInt", dwCreationFlags,
        "Ptr", lpThreadId
    )
}

SuspendThread(hThread)
{
    return DllCall(
        "SuspendThread",
        "Ptr", hThread
    )
}

ResumeThread(hThread)
{
    return DllCall(
        "ResumeThread",
        "Ptr", hThread
    )
}

ClipCursor(lpRect)
{
    return DllCall(
        "ClipCursor",
        "Ptr", lpRect,
    )
}

Sleep(dwMilliSeconds)
{
    DllCall(
        "Sleep",
        "UInt", dwMilliSeconds
    )
}


