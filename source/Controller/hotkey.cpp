#include <Windows.h>
#include <WinUser.h>
#include <cstddef>
#include <errhandlingapi.h>
#include <imm.h>
#include <iostream>
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
    bSuccess = bSuccess && RegisterHotKey(NULL, 0, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '0');
    bSuccess = bSuccess && RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '1');
    bSuccess = bSuccess && RegisterHotKey(NULL, 2, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '2');
    bSuccess = bSuccess && RegisterHotKey(NULL, 3, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '3');
    bSuccess = bSuccess && RegisterHotKey(NULL, 4, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '4');
    bSuccess = bSuccess && RegisterHotKey(NULL, 5, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '5');
    if (!bSuccess)
    {
        ConsoleLog("【错误】线程 hHandleHotKeyMessageThread 注册热键时失败。\r\n");
        return 0;
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (msg.message == WM_HOTKEY && msg.wParam >= 0 && msg.wParam <= 5)
        {
            ResetEvent(hEnableWatchGameStateEvent);
            ResetEvent(hEnableWatchGameProcessEvent);
            ResetEvent(hEnableCombinePartsEvent);
            ResetEvent(hEnablePurchaseItemEvent);
            ResetEvent(hEnableLocateCursorEvent);
            if (msg.wParam == 0)
            {
                ConsoleLog("【消息】切换为 0 模式\r\n");
                GiveCommand(LUA_CMD_NOP); /* 写入空命令，LUA 脚本暂停运行 */
            }
            else if (msg.wParam == 1)
            {
                CSOL24H::DisableExtendedMode();
                SetEvent(hEnableWatchGameStateEvent);
                SetEvent(hEnableWatchGameProcessEvent);
                ConsoleLog("【消息】切换为 1 模式。\r\n");
            }
            else if (msg.wParam == 2)
            {
                CSOL24H::EnableExtendedMode();
                SetEvent(hEnableWatchGameStateEvent);
                SetEvent(hEnableWatchGameProcessEvent);
                ConsoleLog("【消息】切换为 2 模式。\r\n");
            }
            else if (msg.wParam == 3) /* 合成配件 */
            {
                SetEvent(hEnableCombinePartsEvent);
                ConsoleLog("【消息】切换为 3 模式。\r\n");
                GiveCommand(LUA_CMD_COMBINE_PARTS);
            }
            else if (msg.wParam == 4) /* 购买物品 */
            {
                SetEvent(hEnablePurchaseItemEvent);
                ConsoleLog("【消息】切换为 4 模式。\r\n");
            }
            else if (msg.wParam == 5) /* 光标定位 */
            {
                SetEvent(hEnableLocateCursorEvent);
                ConsoleLog("【消息】切换为 5 模式。\r\n");
            }
            ReleaseMutex(hRunnableMutex);
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterHotKey(NULL, 0);
    UnregisterHotKey(NULL, 1);
    UnregisterHotKey(NULL, 2);
    UnregisterHotKey(NULL, 3);
    UnregisterHotKey(NULL, 4);
    UnregisterHotKey(NULL, 5);
    ConsoleLog("【消息】线程 hHandleHotKeyMessageThread 退出。\r\n");
    return 0;
}