#include "Console.hpp"
#include "CSOL24H.hpp"
#include <Windows.h>
#include <cstddef>
#include <handleapi.h>
#include <iostream>
#include <minwindef.h>
#include <synchapi.h>

HANDLE hConsoleMutex = NULL;

static BOOL HandlerRoutine(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT)
    {
        CSOL24H::Destroy();
        DestroyConsole();
        return TRUE;
    }
    else if (dwCtrlType == CTRL_CLOSE_EVENT)
    {
        CSOL24H::Destroy();
        DestroyConsole();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool InitializeConsole() noexcept
{
    hConsoleMutex = CreateMutexW(NULL, FALSE, NULL);
    if (!hConsoleMutex)
    {
        std::printf("【错误】创建控制台互斥体失败。错误代码 %lu。\r\n", GetLastError());
        return false;
    }
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
    std::puts("【消息】控制台代码页及区域格式设为 UTF-8。");
    if (!SetConsoleCtrlHandler(HandlerRoutine, TRUE))
    {
        std::printf("【错误】注册控制台控制信号处理函数失败。错误代码 %lu。\r\n", GetLastError());
        return false;
    }
    std::puts("【消息】注册控制台控制信号处理函数。");
    return true;
}


void DestroyConsole() noexcept
{
    WaitForSingleObject(hConsoleMutex, 500);
    CloseHandle(hConsoleMutex);
}