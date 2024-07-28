#include "Console.hpp"
#include "CSOL24H.hpp"
#include <Windows.h>
#include <cstddef>
#include <errhandlingapi.h>
#include <handleapi.h>
#include <iostream>
#include <minwindef.h>
#include <processenv.h>
#include <synchapi.h>
#include <wincon.h>

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
        std::printf("【错误】创建控制台互斥体失败。错误代码：%lu。\r\n", GetLastError());
        return false;
    }
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
    std::puts("【消息】控制台代码页及区域格式设为 UTF-8。");
    if (!SetConsoleCtrlHandler(HandlerRoutine, TRUE))
    {
        std::printf("【错误】注册控制台控制信号处理函数失败。错误代码：%lu。\r\n", GetLastError());
        return false;
    }
    std::puts("【消息】注册控制台控制信号处理函数。");
    DWORD dwMode;
    if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dwMode))
    {
        std::printf("【消息】获取控制台输入模式失败。错误代码：%lu。\r\n", GetLastError());
        return false;
    }
    if (dwMode & ENABLE_QUICK_EDIT_MODE)
    {
        std::puts("【消息】检测到快速编辑模式开启，尝试关闭快速编辑模式。");
        if (!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), dwMode & ~ENABLE_QUICK_EDIT_MODE))
        {
            std::printf("【错误】设置控制台输入模式时出现错误。错误代码：%lu。\r\n", GetLastError());
            return false;
        }
        std::puts("【消息】成功禁用控制台快速编辑模式。");
    }
    else
    {
        std::puts("【消息】检测到控制台快速编辑模式已禁用。");
    }
    return true;
}


void DestroyConsole() noexcept
{
    WaitForSingleObject(hConsoleMutex, 500);
    CloseHandle(hConsoleMutex);
}