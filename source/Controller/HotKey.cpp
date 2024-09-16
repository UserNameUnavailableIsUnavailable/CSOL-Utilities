#include <Windows.h>
#include <WinUser.h>
#include <cstddef>
#include <errhandlingapi.h>
#include <imm.h>
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
    static int32_t mode = '0';
    bSuccess = bSuccess && RegisterHotKey(NULL, '0', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '0');
    bSuccess = bSuccess && RegisterHotKey(NULL, '1', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '1');
    bSuccess = bSuccess && RegisterHotKey(NULL, '2', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '2');
    bSuccess = bSuccess && RegisterHotKey(NULL, '3', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '3');
    bSuccess = bSuccess && RegisterHotKey(NULL, '4', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '4');
    bSuccess = bSuccess && RegisterHotKey(NULL, '5', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '5');
    if (!bSuccess)
    {
        ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_ERROR, "线程 hHandleHotKeyMessageThread 注册热键时失败，请检查是否存在热键冲突。错误代码：%lu。", GetLastError());
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
            if (msg.wParam != '1' && msg.wParam != '2')
            {
                bWasWatchGameProcessStateThreadPaused = true;
            }
            if (msg.wParam == '0')
            {
                ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 0 模式");
                GiveCommand(LUA_CMD_NOP); /* 写入空命令，LUA 脚本暂停运行 */
            }
            else if (msg.wParam == '1')
            {
                CSOL24H::DisableExtendedMode();
                SetEvent(hEnableWatchGameProcessEvent);
                SetEvent(hEnableWatchInGameStateEvent);
                ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 1 模式。");
            }
            else if (msg.wParam == '2')
            {
                CSOL24H::EnableExtendedMode();
                SetEvent(hEnableWatchGameProcessEvent);
                SetEvent(hEnableWatchInGameStateEvent);
                ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 2 模式。");
            }
            else if (msg.wParam == '3') /* 合成配件 */
            {
                SetEvent(hEnableCombinePartsEvent);
                ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 3 模式。");
                GiveCommand(LUA_CMD_COMBINE_PARTS);
            }
            else if (msg.wParam == '4') /* 购买物品 */
            {
                SetEvent(hEnablePurchaseItemEvent);
                ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 4 模式。");
            }
            else if (msg.wParam == '5') /* 光标定位 */
            {
                SetEvent(hEnableLocateCursorEvent);
                ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 5 模式。");
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterHotKey(NULL, '0');
    UnregisterHotKey(NULL, '1');
    UnregisterHotKey(NULL, '2');
    UnregisterHotKey(NULL, '3');
    UnregisterHotKey(NULL, '4');
    UnregisterHotKey(NULL, '5');
    ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 hHandleHotKeyMessageThread 退出。");
    return 0;
}