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
        std::cout << "【错误】线程 hHandleHotKeyMessageThread 注册热键时失败。" << std::endl;
        return 0;
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (msg.message == WM_HOTKEY && msg.wParam >= 0 && msg.wParam <= 5)
        {
            if (WAIT_OBJECT_0 != WaitForSingleObject(hRunnableMutex, INFINITE)) /* 等待正运行的线程释放互斥体后才继续运行热键处理线程 */
            {
                break; /* 出现异常，线程只能退出 */
            }

            if (msg.wParam == 0)
            {
                ResetEvent(hEnableWatchGameStateEvent);
                ResetEvent(hEnableWatchGameProcessEvent);
                std::cout << "【消息】切换为 0 模式" << std::endl;
                GiveCommand(LUA_CMD_NOP); /* 写入空命令，LUA 脚本暂停运行 */
                ReleaseMutex(hRunnableMutex); /* 释放互斥体 */
            }
            else if (msg.wParam == 1)
            {
                // SetEvent(hEnableWatchGameStateEvent);
                SetEvent(hEnableWatchGameProcessEvent);
                std::cout << "【消息】切换为 1 模式" << std::endl;
                ReleaseMutex(hRunnableMutex);
            }
            else if (msg.wParam == 2)
            {
                SetEvent(hEnableWatchGameStateEvent);
                std::cout << "【消息】切换为 2 模式" << std::endl;
            }
            else if (msg.wParam == 3) /* 合成配件 */
            {

            }
            else if (msg.wParam == 4) /* 购买物品 */
            {

            }
            else if (msg.wParam == 5) /* 光标定位 */
            {

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
    std::cout << "【消息】线程 hHandleHotKeyMessageThread 退出。" << std::endl;
    return 0;
}