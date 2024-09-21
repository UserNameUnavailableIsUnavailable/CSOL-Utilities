#include <Windows.h>
#include <CSOL_Utilities.hpp>
#include "CController.hpp"
#include "CConsole.hpp"

using namespace CSOL_Utilities;

void CController::HandleHotKeyEvent() noexcept
{
    BOOL bSuccess = TRUE;
    uint32_t count = -1;
    thread_local int32_t mode = 0;
    bSuccess = bSuccess && RegisterHotKey(NULL, 0, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '0');
    bSuccess = bSuccess && RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '1');
    bSuccess = bSuccess && RegisterHotKey(NULL, 2, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '2');
    bSuccess = bSuccess && RegisterHotKey(NULL, 3, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '3');
    bSuccess = bSuccess && RegisterHotKey(NULL, 4, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '4');
    bSuccess = bSuccess && RegisterHotKey(NULL, 5, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '5');
    if (!bSuccess) {
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, "注册热键时失败，请检查是否存在热键冲突。错误代码：%lu。", GetLastError());
        goto fail;
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_HOTKEY && mode != msg.wParam /* 需要进行模式切换 */ && msg.wParam >= 0 && msg.wParam <= 5) {
            if (mode == 1) {
                s_Instance->m_InGameStateWatcherSwitch.Reset();
                s_Instance->m_GameProcessWatcherSwitch.Reset();
                s_Instance->m_InGameStateWatcherFinished.Wait();
                s_Instance->m_GameProcessWatcherFinished.Wait();
            } else if (mode == 2) {
                s_Instance->m_InGameStateWatcherSwitch.Reset();
                s_Instance->m_GameProcessWatcherSwitch.Reset();
                s_Instance->m_InGameStateWatcherFinished.Wait();
                s_Instance->m_GameProcessWatcherFinished.Wait();
            } else if (mode == 3) {
                s_Instance->m_FixedCommandDispatcherSwitch.Reset();
            } else if (mode == 4) {
                s_Instance->m_FixedCommandDispatcherSwitch.Reset();
            } else if (mode == 5) {
                s_Instance->m_FixedCommandDispatcherSwitch.Reset();
            }
            /* 切换状态 */
            if (msg.wParam == 0) {
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "0 号模式。");
            } else if (msg.wParam == 1) {
                s_Instance->m_InGameStateWatcherSwitch.Set();
                s_Instance->m_GameProcessWatcherSwitch.Set();
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "1 号模式。");
            } else if (msg.wParam == 2) {
                s_Instance->m_InGameStateWatcherSwitch.Set();
                s_Instance->m_GameProcessWatcherSwitch.Set();
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "2 号模式。");
            } else if (msg.wParam == 3) { /* 合成配件 */
                s_Instance->m_Command = CMD_COMBINE_PARTS;
                s_Instance->m_FixedCommandDispatcherSwitch.Set();
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "3 号模式。");
            } else if (msg.wParam == 4) { /* 购买物品 */
                s_Instance->m_Command = CMD_PURCHASE_ITEM;
                s_Instance->m_FixedCommandDispatcherSwitch.Set();
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "4 号模式。");
            } else if (msg.wParam == 5) { /* 光标定位 */
                s_Instance->m_Command = CMD_LOCATE_CURSOR;
                s_Instance->m_FixedCommandDispatcherSwitch.Set();
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "5 号模式。");
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
fail:
    UnregisterHotKey(NULL, 0);
    UnregisterHotKey(NULL, 1);
    UnregisterHotKey(NULL, 2);
    UnregisterHotKey(NULL, 3);
    UnregisterHotKey(NULL, 4);
    UnregisterHotKey(NULL, 5);
    CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 m_HotKeyEventHandler 退出。");
    s_Instance->m_bThreadExit.Set();
}