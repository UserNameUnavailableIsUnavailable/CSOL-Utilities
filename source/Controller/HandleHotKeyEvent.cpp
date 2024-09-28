#include "CConsole.hpp"
#include "CController.hpp"
#include <CSOL_Utilities.hpp>
#include <Windows.h>
#include <cassert>
#include <errhandlingapi.h>

using namespace CSOL_Utilities;

void CController::HandleHotKeyEvent() noexcept
{
    BOOL bSuccess = TRUE;
    thread_local int32_t mode = '0';
    const char *conflict_hotkey{nullptr};
    uint32_t error_code{0};
    if (!RegisterHotKey(nullptr, '0', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '0'))
    {
        conflict_hotkey = "Ctrl Alt 0";
        goto exit_this_thread_error;
    }
    else if (!RegisterHotKey(nullptr, '1', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '1'))
    {
        conflict_hotkey = "Ctrl Alt 1";
        goto unreg_hotkey_0;
    }
    else if (!RegisterHotKey(nullptr, '2', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '2'))
    {
        conflict_hotkey = "Ctrl Alt 2";
        goto unreg_hotkey_1;
    }
    else if (!RegisterHotKey(nullptr, '3', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '3'))
    {
        conflict_hotkey = "Ctrl Alt 3";
        goto unreg_hotkey_2;
    }
    else if (!RegisterHotKey(nullptr, '4', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '4'))
    {
        conflict_hotkey = "Ctrl Alt 4";
        goto unreg_hotkey_3;
    }
    else if (!RegisterHotKey(nullptr, '5', MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '5'))
    {
        conflict_hotkey = "Ctrl Alt 5";
        goto unreg_hotkey_4;
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (msg.message == WM_HOTKEY && mode != msg.wParam /* 需要进行模式切换 */ && msg.wParam >= '0' &&
            msg.wParam <= '5')
        {
            if (mode == '1')
            {
                s_Instance->m_InGameStateWatcherSwitch.Reset();
                s_Instance->m_InGameStateWatcherFinished.Wait();
                s_Instance->m_GameProcessWatcherSwitch.Reset();
                s_Instance->m_GameProcessWatcherFinished.Wait();
            }
            else if (mode == '2')
            {
                s_Instance->m_InGameStateWatcherSwitch.Reset();
                s_Instance->m_InGameStateWatcherFinished.Wait();
                s_Instance->m_GameProcessWatcherSwitch.Reset();
                s_Instance->m_GameProcessWatcherFinished.Wait();
            }
            else if (mode == '3')
            {
                s_Instance->m_FixedCommandDispatcherSwitch.Reset();
            }
            else if (mode == '4')
            {
                s_Instance->m_FixedCommandDispatcherSwitch.Reset();
            }
            else if (mode == '5')
            {
                s_Instance->m_FixedCommandDispatcherSwitch.Reset();
            }
            /* 切换状态 */
            if (msg.wParam == '0')
            {
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 0 号模式。");
                s_Instance->m_Messenger.AssignAndDispatch(COMMAND::CMD_NOP, 0);
            }
            else if (msg.wParam == '1')
            {
                s_Instance->m_InGameStateWatcherSwitch.Set();
                s_Instance->m_GameProcessWatcherSwitch.Set();
                s_Instance->SetExtendedAutoPlayMode(false);
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 1 号模式。");
            }
            else if (msg.wParam == '2')
            {
                s_Instance->m_InGameStateWatcherSwitch.Set();
                s_Instance->m_GameProcessWatcherSwitch.Set();
                s_Instance->SetExtendedAutoPlayMode(true);
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 2 号模式。");
            }
            else if (msg.wParam == '3')
            { /* 合成配件 */
                s_Instance->m_Command = CMD_COMBINE_PARTS;
                s_Instance->m_FixedCommandDispatcherSwitch.Set();
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 3 号模式。");
            }
            else if (msg.wParam == '4')
            { /* 购买物品 */
                s_Instance->m_Command = CMD_PURCHASE_ITEM;
                s_Instance->m_FixedCommandDispatcherSwitch.Set();
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 4 号模式。");
            }
            else if (msg.wParam == '5')
            { /* 光标定位 */
                s_Instance->m_Command = CMD_LOCATE_CURSOR;
                s_Instance->m_FixedCommandDispatcherSwitch.Set();
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "切换为 5 号模式。");
            }
            mode = msg.wParam;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
unreg_hotkey_5:
    UnregisterHotKey(nullptr, '5');
unreg_hotkey_4:
    UnregisterHotKey(nullptr, '4');
unreg_hotkey_3:
    UnregisterHotKey(nullptr, '3');
unreg_hotkey_2:
    UnregisterHotKey(nullptr, '2');
unreg_hotkey_1:
    UnregisterHotKey(nullptr, '1');
unreg_hotkey_0:
    UnregisterHotKey(nullptr, '0');
exit_this_thread_error:
    if (conflict_hotkey)
    {
        error_code = GetLastError();
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR,
                      "尝试注册全局热键“%s”时失败，请检查是否存在热键冲突。错误代码：%lu。", conflict_hotkey,
                      error_code);
    }
exit_this_thread_normal:
    CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 m_HotKeyEventHandler 退出。");
    s_Instance->m_ThreadExitEvent.Set();
}