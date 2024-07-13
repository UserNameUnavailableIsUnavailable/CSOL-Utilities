#include <Windows.h>
#include <WinUser.h>
#include <errhandlingapi.h>
#include <imm.h>
#include <iostream>
#include "CSOL24H.hpp"

DWORD CSOL24H::HandleHotKey(LPVOID lpParam) noexcept
{
    BOOL bSuccess = TRUE;
    bSuccess = bSuccess && RegisterHotKey(nullptr, 0, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '0');
    bSuccess = bSuccess && RegisterHotKey(nullptr, 1, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '1');
    bSuccess = bSuccess && RegisterHotKey(nullptr, 2, MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, '2');
    if (!bSuccess)
    {
        return 0;
    }
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (msg.message == WM_HOTKEY)
        {
            if (msg.wParam == 0)
            {
                std::cout << "HotKey 0" << std::endl;
            }
            else if (msg.wParam == 1)
            {
                std::cout << "HotKey 1" << std::endl;
            }
            else if (msg.wParam == 2)
            {
                std::cout << "HotKey 2" << std::endl;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterHotKey(nullptr, 0);
    UnregisterHotKey(nullptr, 1);
    UnregisterHotKey(nullptr, 2);
    return 0;
}