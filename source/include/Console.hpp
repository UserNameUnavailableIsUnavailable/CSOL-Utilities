#include <Windows.h>
#include <iostream>

extern HANDLE hConsoleMutex;

bool InitializeConsole() noexcept;
template <typename... ARG>
int ConsoleLog(const char* fmt, const ARG&... args) noexcept
{
    if (WAIT_OBJECT_0 == WaitForSingleObject(hConsoleMutex, INFINITE))
    {
        auto ret = std::printf(fmt, args...);
        ReleaseMutex(hConsoleMutex);
        return ret;
    }
    return 0;
}
void DestroyConsole() noexcept;