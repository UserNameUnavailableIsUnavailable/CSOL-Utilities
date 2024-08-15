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
    /* 先设置代码页和 locale 以便处理中文输入 */
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
    std::puts("【消息】控制台代码页及区域格式设为 UTF-8。");
    
    /* 控制台信号处理 */
    if (!SetConsoleCtrlHandler(HandlerRoutine, TRUE))
    {
        std::printf("【错误】注册控制台控制信号处理函数失败。错误代码：%lu。\r\n", GetLastError());
        return false;
    }
    std::puts("【消息】注册控制台控制信号处理函数。");

    /* 控制台互斥体 */
    hConsoleMutex = CreateMutexW(NULL, FALSE, NULL);
    if (!hConsoleMutex)
    {
        std::printf("【错误】创建控制台互斥体失败。错误代码：%lu。\r\n", GetLastError());
        return false;
    }

    DWORD dwInputMode, dwOutputMode;
    /* 设置控制台输入模式 */
    if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dwInputMode))
    {
        std::printf("【错误】获取控制台输入模式失败。错误代码：%lu。\r\n", GetLastError());
        return false;
    }
    if (dwInputMode & ENABLE_QUICK_EDIT_MODE) /* 快速编辑处于打开状态 */
    {
        dwInputMode &= ~ENABLE_QUICK_EDIT_MODE; /* 关闭快速编辑 */
        std::puts("【消息】检测到快速编辑模式开启，尝试关闭快速编辑模式。");
        if (!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), dwInputMode))
        {
            std::printf("【错误】设置控制台输入模式时出现错误。错误代码：%lu。\r\n", GetLastError());
            return false;
        }
    }
    else
    {
        std::puts("【消息】检测到快速编辑模式已经关闭。");
    }

    /* 设置控制台输出模式 */
    if (!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwOutputMode))
    {
        std::printf("【错误】获取控制台输出模式失败。错误代码：%lu。\r\n", GetLastError());
        return false;
    }
    if (!(dwOutputMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) /* 虚拟终端处理处于关闭状态 */
    {
        dwOutputMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; /* 开启虚拟终端处理 */
        std::puts("【消息】检测到虚拟终端处理处于关闭状态，尝试开启虚拟终端处理。");
        if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwOutputMode))
        {
            std::printf("【错误】设置控制台输出模式时出现错误。错误代码：%lu。\r\n", GetLastError());
            return false;
        }
    }
    else
    {
        std::puts("【消息】检测到虚拟终端处理已经开启。");
    }
    return true;
}


void DestroyConsole() noexcept
{
    WaitForSingleObject(hConsoleMutex, 500);
    CloseHandle(hConsoleMutex);
}