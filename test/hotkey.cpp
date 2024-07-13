#include <Windows.h>
#include <iostream>
int main()
{
    if (!RegisterHotKey(nullptr, 0, MOD_ALT | MOD_NOREPEAT | MOD_SHIFT, '0') ||
    !RegisterHotKey(nullptr, 1, MOD_ALT | MOD_NOREPEAT | MOD_SHIFT, '1'))
    {
        std::cout << "Error " << GetLastError() << std::endl;
        return -1;
    }
    MSG msg {0};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (msg.message == WM_HOTKEY)
        {
            std::cout << "hotkey: " << std::endl;
            if (msg.wParam == 0)
            {
                std::cout << "id = 0" << std::endl;
                break;
            }
            else if (msg.wParam == 1)
            {
                std::cout << "id = 1" << std::endl;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterHotKey(nullptr, 0);
    UnregisterHotKey(nullptr, 1);
    return 0;
}
