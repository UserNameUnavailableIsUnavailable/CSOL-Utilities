#include <Windows.h>
#include <cstdio>

extern HANDLE hConsoleMutex;

bool InitializeConsole() noexcept;

enum ENUM_CONSOLE_LOG_LEVEL
{
    CLL_MESSAGE,
    CLL_WARNING,
    CLL_ERROR
};
template <typename... ARG>
int ConsoleLog(ENUM_CONSOLE_LOG_LEVEL cll, const char* fmt, const ARG&... args) noexcept
{
    int ret;
    if (WAIT_OBJECT_0 == WaitForSingleObject(hConsoleMutex, INFINITE))
    {
        if (cll == ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE)
        {
            std::printf("【消息】");
            std::printf(fmt, args...);
            std::printf("\r\n");
        }
        else if (cll == ENUM_CONSOLE_LOG_LEVEL::CLL_ERROR)
        {
            std::printf("\x1b[91m【错误】"); /* 红色字体打印错误 */
            ret = std::printf(fmt, args...);
            std::printf("\x1b[39m\r\n");
        }
        else if (cll == ENUM_CONSOLE_LOG_LEVEL::CLL_WARNING)
        {
            std::printf("\x1b[93m【警告】"); /* 黄色字体打印警告 */
            ret = std::printf(fmt, args...);
            std::printf("\x1b[39m\r\n");
        }
        ReleaseMutex(hConsoleMutex);
        return ret;
    }
    return 0;
}
void DestroyConsole() noexcept;