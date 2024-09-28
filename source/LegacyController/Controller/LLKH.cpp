#include <Windows.h>
#include <libloaderapi.h>
#include <minwinbase.h>
#include "CSOL24H.hpp"

LRESULT CALLBACK CSOL24H::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode != HC_ACTION)  // do not process message 
        return CallNextHookEx(hLLKH, nCode, wParam, lParam); 
 
    auto p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (wParam == WM_KEYDOWN || wParam == WM_KEYUP)
    {
        if (p->vkCode == VK_LWIN) /* LWin */
        {
        }
        if (p->vkCode == VK_OEM_3) return 1; /* ` */
        if (p->vkCode == VK_ESCAPE && (GetAsyncKeyState(VK_LMENU) & 0x8000)) return 1; /* Alt Esc */
        if (p->vkCode == VK_F4 && (GetAsyncKeyState(VK_LMENU) &  0x8000)) return 1; /* Alt F4 */
        if (p->vkCode == VK_TAB && (GetAsyncKeyState(VK_LMENU) & 0x8000)) return 1; /* Alt Tab */
        if (p->vkCode == VK_TAB && (GetAsyncKeyState(VK_LWIN) & 0x8000)) return 1; /* Win Tab */
        static DWORD last_apps_pressed_time = 0;
        if (p->vkCode == VK_APPS && p->time - last_apps_pressed_time > 100)
        {
            auto function = GetProcAddress(hGamingToolModule, "ChangeForegroundWindowInputLanguage");
            last_apps_pressed_time = p->time;
            if (function) function();
            return 1;
        }
        if (p->vkCode == 'T' && ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000)))
        {
            auto function = (void(*)(void))GetProcAddress(hGamingToolModule, "ToggleTray");
            if (function) function();
            return 1;
        }
    }
    return CallNextHookEx(hLLKH, nCode, wParam, lParam);
}