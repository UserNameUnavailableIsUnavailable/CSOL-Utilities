#include "CSOL24H.hpp"
#include "Console.hpp"

DWORD CSOL24H::StopCSOBanner(LPVOID lpParam) noexcept
{
    while (!bDestroy)
    {
        HWND hWnd = FindWindowW(NULL, L"CSOBanner");
        DWORD dwPid = 0;
        HANDLE hProc = NULL;
        GetWindowThreadProcessId(hWnd, &dwPid);
        if (dwPid)
        {
            hProc = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwPid);
        }
        if (hProc)
        {
            TerminateProcess(hProc, -1);
            CloseHandle(hProc);
        }
        Sleep(1000);
    }
    ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 hStopCSOBannerThread 退出");
    return 0;
}