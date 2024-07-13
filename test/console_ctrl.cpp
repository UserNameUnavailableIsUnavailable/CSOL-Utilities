#include <Windows.h>
#include <wincon.h>
#include <iostream>

bool g_bExit = false;

BOOL CtrlEventHandler(DWORD dwType)
{
    return TRUE;
}

int main()
{
    SetConsoleCtrlHandler(CtrlEventHandler, TRUE);
    Sleep(3000);
    std::cout << "exit" << std::endl;
    return 0;
}