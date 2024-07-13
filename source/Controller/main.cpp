#include "CSOL24H.hpp"
#include "CSOL24H_EXCEPT.hpp"
#include <cstdio>
#include <cwchar>
#include <iostream>
#include <wincon.h>

bool g_Exit = false;
BOOL HandlerRoutine(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT)
    {
        CSOL24H::Destroy();
        std::puts("【消息】退出。");
        g_Exit = true;
        return TRUE;
    }
    else if (dwCtrlType == CTRL_CLOSE_EVENT)
    {
        CSOL24H::Destroy();
        std::puts("【消息】退出。");
        g_Exit = TRUE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
    std::puts("【消息】控制台代码页及区域格式设为 UTF-8。");
    if (!SetConsoleCtrlHandler(HandlerRoutine, TRUE))
    {
        std::printf("【错误】注册控制台控制信号处理函数失败，错误代码 %lu。按任意键退出。\r\n", GetLastError());
        std::getchar();
        return 0;
    }
    std::puts("【消息】注册控制台控制信号处理函数。");
    try
    {
        CSOL24H::Initialize();
    }
    catch (CSOL24H_EXCEPT e)
    {
        std::cout << e.what() << std::endl;
        CSOL24H::Destroy();
        std::printf("【消息】发生错误，无法继续运行。按任意键退出。");
        std::getchar();
    }
}