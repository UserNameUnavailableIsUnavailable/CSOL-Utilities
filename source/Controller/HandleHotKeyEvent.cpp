#include <Windows.h>
#include <CSOL_Utilities.hpp>
#include "CCommander.hpp"
#include "CConsole.hpp"

using namespace CSOL_Utilities;

void CCommander::HandleHotKeyEvent() noexcept
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
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_HOTKEY && msg.wParam >= 0 && msg.wParam <= 5) {
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
            } else if (mode == 4) {
            } else if (mode == 5) {
            }
            /* 切换状态 */
            if (msg.wParam != '1' && msg.wParam != '2') {
            }
            if (msg.wParam == '0') {
            } else if (msg.wParam == '1') {
            } else if (msg.wParam == '2') {
            } else if (msg.wParam == '3') { /* 合成配件 */
            }
            else if (msg.wParam == '4') { /* 购买物品 */
            } else if (msg.wParam == '5') { /* 光标定位 */
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
    CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 m_HotKeyEventHandler 退出。");
}