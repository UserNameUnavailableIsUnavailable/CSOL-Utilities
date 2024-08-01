#include <Windows.h>
#include <WinUser.h>
#include <cstddef>
#include <errhandlingapi.h>
#include <imm.h>
#include <iostream>
#include <libloaderapi.h>
#include <minwindef.h>
#include <synchapi.h>
#include <winnt.h>
#include <winreg.h>
#include "CSOL24H.hpp"
#include "Command.hpp"
#include "Console.hpp"

DWORD CSOL24H::HandleHotKey(LPVOID lpParam) noexcept
{
    BOOL bSuccess = TRUE;
    uint32_t count = -1;

    // HHOOK hHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    // if (!hHook)
    // {
    //     ConsoleLog("【错误】尝试屏蔽键盘热键失败。错误代码：%lu。", GetLastError());
    //     return -1;
    // }
    bSuccess = bSuccess && RegisterHotKey(NULL, '0', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '0');
    bSuccess = bSuccess && RegisterHotKey(NULL, '1', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '1');
    bSuccess = bSuccess && RegisterHotKey(NULL, '2', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '2');
    bSuccess = bSuccess && RegisterHotKey(NULL, '3', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '3');
    bSuccess = bSuccess && RegisterHotKey(NULL, '4', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '4');
    bSuccess = bSuccess && RegisterHotKey(NULL, '5', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '5');
    // bSuccess = bSuccess && RegisterHotKey(NULL, 6, MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, 'T'); /* GamingTool 功能隐藏任务栏 */
    // bSuccess = bSuccess && RegisterHotKey(NULL, 7, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'B'); /* GamingTool 功能 Ctrl Alt B */
    // bSuccess = bSuccess && RegisterHotKey(NULL, 8, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, 'B'); /* GamingTool 功能 Ctrl Alt Shift B */
    // bSuccess = bSuccess && RegisterHotKey(NULL, 9, MOD_WIN | MOD_NOREPEAT, 'T'); /* GamingTool 功能 Win T */
    // bSuccess = bSuccess && RegisterHotKey(NULL, 10, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'H'); /* GamingTool 功能 Ctrl Alt H */
    // bSuccess = bSuccess && RegisterHotKey(NULL, 11, MOD_WIN | MOD_NOREPEAT, VK_DOWN); /* GamingTool 功能 Win DOWN */
    // bSuccess = bSuccess && RegisterHotKey(NULL, 12, MOD_WIN | MOD_NOREPEAT, VK_UP); /* GamingTool 功能 Win UP */
    if (!bSuccess)
    {
        ConsoleLog("【错误】线程 hHandleHotKeyMessageThread 注册热键时失败。错误代码：%lu。\r\n");
        return -1;
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (msg.message == WM_HOTKEY && msg.wParam >= '0' && msg.wParam <= '5')
        {
            ResetEvent(hEnableWatchInGameStateEvent);
            ResetEvent(hEnableWatchGameProcessEvent);
            ResetEvent(hEnableCombinePartsEvent);
            ResetEvent(hEnablePurchaseItemEvent);
            ResetEvent(hEnableLocateCursorEvent);
            if (msg.wParam == '0')
            {
                ConsoleLog("【消息】切换为 0 模式\r\n");
                GiveCommand(LUA_CMD_NOP); /* 写入空命令，LUA 脚本暂停运行 */
            }
            else if (msg.wParam == '1')
            {
                CSOL24H::DisableExtendedMode();
                SetEvent(hEnableWatchGameProcessEvent);
                ConsoleLog("【消息】切换为 1 模式。\r\n");
            }
            else if (msg.wParam == '2')
            {
                CSOL24H::EnableExtendedMode();
                SetEvent(hEnableWatchGameProcessEvent);
                ConsoleLog("【消息】切换为 2 模式。\r\n");
            }
            else if (msg.wParam == '3') /* 合成配件 */
            {
                SetEvent(hEnableCombinePartsEvent);
                ConsoleLog("【消息】切换为 3 模式。\r\n");
                GiveCommand(LUA_CMD_COMBINE_PARTS);
            }
            else if (msg.wParam == '4') /* 购买物品 */
            {
                SetEvent(hEnablePurchaseItemEvent);
                ConsoleLog("【消息】切换为 4 模式。\r\n");
            }
            else if (msg.wParam == '5') /* 光标定位 */
            {
                SetEvent(hEnableLocateCursorEvent);
                ConsoleLog("【消息】切换为 5 模式。\r\n");
            }
        }
        // else if (msg.message == WM_HOTKEY && msg.wParam >= 6 && msg.wParam <= 14)
        // {
        //     if (msg.wParam == 6)
        //     {
        //         auto function = (void(*)(void))GetProcAddress(hGamingToolModule, "ToggleTray");
        //         if (function) function();
        //     }
        //     else if (msg.wParam == 7)
        //     {
        //         auto function = (void(*)(void))GetProcAddress(hGamingToolModule, "MakeForegroundWindowBorderless");
        //         if (function) function();
        //     }
        //     else if (msg.wParam == 8)
        //     {
        //         auto function = (void(*)(void))GetProcAddress(hGamingToolModule, "MakeForegroundWindowBordered");
        //         if (function) function();
        //     }
        //     else if (msg.wParam == 9)
        //     {
        //         auto function = (void(*)(void))GetProcAddress(hGamingToolModule, "ToggleTopmostForegroundWindow");
        //         if (function) function();
        //     }
        //     else if (msg.wParam == 10)
        //     {
        //         auto function = (void(*)(void))GetProcAddress(hGamingToolModule, "ToggleCursorClipper");
        //         if (function) function();
        //     }
        //     else if (msg.wParam == 11)
        //     {
        //         auto function = (void(*)(void))GetProcAddress(hGamingToolModule, "MinimizeForegroundWindow");
        //         if (function) function();
        //     }
        //     else if (msg.wParam == 12)
        //     {
        //         auto function = (void(*)(void))GetProcAddress(hGamingToolModule, "RestoreMinimizedWindow");
        //         if (function) function();
        //     }
        // }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterHotKey(NULL, '0');
    UnregisterHotKey(NULL, '1');
    UnregisterHotKey(NULL, '2');
    UnregisterHotKey(NULL, '3');
    UnregisterHotKey(NULL, '4');
    UnregisterHotKey(NULL, '5');
    // UnregisterHotKey(NULL, 6);
    // UnregisterHotKey(NULL, 7);
    // UnregisterHotKey(NULL, 8);
    // UnregisterHotKey(NULL, 9);
    // UnregisterHotKey(NULL, 10);
    // UnregisterHotKey(NULL, 11);
    // UnregisterHotKey(NULL, 12);

    // UnhookWindowsHookEx(hHook);

    ConsoleLog("【消息】线程 hHandleHotKeyMessageThread 退出。\r\n");
    return 0;
}