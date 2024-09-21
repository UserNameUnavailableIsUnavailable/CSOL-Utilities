#include "CConsole.hpp"
#include "CException.hpp"
#include <iostream>
#include <clocale>

using namespace CSOL_Utilities;

std::mutex CConsole::m_Mutex{ };

int CConsole::Log(CONSOLE_LOG_LEVEL level, const char* string) noexcept
{
    int ret;
    if (level == CONSOLE_LOG_LEVEL::CLL_MESSAGE) {
        std::cout << "【消息】" << string << std::endl;
    } else if (level == CONSOLE_LOG_LEVEL::CLL_WARNING) {
        std::cout << "\x1b[93m【警告】" /* 黄色字体打印警告 */
        << string << "x1b[39m" << std::endl;
    } else if (level == CONSOLE_LOG_LEVEL::CLL_ERROR) {
        std::cout << "\x1b[91m【错误】" /* 红色字体打印错误 */
            << string << "\x1b[39m" << std::endl;
    } 
    std::cout.flush();
    return ret;
}

bool CConsole::ConfigureConsole(BOOL(*on_destroy_callback)(DWORD dwCtrlType)) noexcept
{
    bool ok = true;
    /* 先设置代码页和 locale 以便处理中文输入 */
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
    std::puts("【消息】控制台代码页及区域格式设为 UTF-8。");
    /* 控制台信号处理 */
    if (!SetConsoleCtrlHandler(on_destroy_callback, TRUE))
    {
        std::printf("【错误】注册控制台控制信号处理函数失败。错误代码：%lu。\r\n", GetLastError());
        ok = false;
        goto finish_config;
    }
    std::puts("【消息】注册控制台控制信号处理函数。");

    DWORD dwInputMode, dwOutputMode;
    /* 设置控制台输入模式 */
    if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dwInputMode))
    {
        std::printf("【警告】获取控制台输入模式失败。错误代码：%lu。\r\n", GetLastError());
        goto finish_config;
    }
    if (dwInputMode & ENABLE_QUICK_EDIT_MODE) /* 快速编辑处于打开状态 */
    {
        dwInputMode &= ~ENABLE_QUICK_EDIT_MODE; /* 关闭快速编辑 */
        std::puts("【消息】检测到快速编辑模式开启，尝试关闭快速编辑模式。");
        if (!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), dwInputMode))
        {
            std::printf("【警告】设置控制台输入模式时出现错误。错误代码：%lu。\r\n", GetLastError());
            goto finish_config;
        }
    }
    else
    {
        std::puts("【消息】检测到快速编辑模式已经关闭。");
    }

    /* 设置控制台输出模式 */
    if (!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwOutputMode))
    {
        std::printf("【警告】获取控制台输出模式失败。错误代码：%lu。\r\n", GetLastError());
        goto finish_config;
    }
    if (!(dwOutputMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) /* 虚拟终端处理处于关闭状态 */
    {
        dwOutputMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; /* 开启虚拟终端处理 */
        std::puts("【消息】检测到虚拟终端处理处于关闭状态，尝试开启虚拟终端处理。");
        if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwOutputMode))
        {
            std::printf("【警告】设置控制台输出模式时出现错误。错误代码：%lu。\r\n", GetLastError());
            goto finish_config;
        }
    }
    else
    {
        std::puts("【消息】检测到虚拟终端处理已经开启。");
    }
finish_config:
    return ok;
}