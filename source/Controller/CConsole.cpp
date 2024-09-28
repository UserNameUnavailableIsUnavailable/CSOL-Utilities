#include "CConsole.hpp"
#include <chrono>
#include <ctime>
#include <iostream>
#include <mutex>

using namespace CSOL_Utilities;

std::mutex CConsole::m_Mutex{ };

void CConsole::Log(CONSOLE_LOG_LEVEL level, std::string s) noexcept
{
    std::lock_guard<std::mutex> lock_guard(m_Mutex);
    std::time_t current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::tm current_tm;
    localtime_s(&current_tm, &current_time);
    char current_time_string[32];
    strftime(current_time_string, sizeof(current_time_string), "%Y/%m/%d %H:%M:%S", &current_tm);
    if (level == CONSOLE_LOG_LEVEL::CLL_MESSAGE) {
        std::cout << "\x1b[92m" << current_time_string << "【消息】" << s << "\x1b[39m" << std::endl;
    } else if (level == CONSOLE_LOG_LEVEL::CLL_WARNING) {
        std::cout << "\x1b[93m" << current_time_string << "【警告】" /* 黄色字体打印警告 */
        << s << "\x1b[39m" << std::endl;
    } else if (level == CONSOLE_LOG_LEVEL::CLL_ERROR) {
        std::cout << "\x1b[91m" << current_time_string << "【错误】" /* 红色字体打印错误 */
            << s << "\x1b[39m" << std::endl;
    } 
}

bool CConsole::Configure(DESTROY_CALLBACK on_destroy) noexcept
{
    bool ok = true;
    /* 先设置代码页和 locale 以便处理中文输入 */
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
    std::puts("控制台代码页及区域格式设为 UTF-8。");
    /* 控制台信号处理 */
    if (!SetConsoleCtrlHandler(on_destroy, TRUE)) {
        std::printf("注册控制台控制信号处理函数失败。错误代码：%lu。\r\n", GetLastError());
        ok = false;
        goto finish_config;
    }
    std::puts("注册控制台控制信号处理函数。");

    DWORD dwInputMode, dwOutputMode;
    /* 设置控制台输入模式 */
    if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dwInputMode)) {
        std::printf("获取控制台输入模式失败。错误代码：%lu。\r\n", GetLastError());
        goto finish_config;
    }
    if (dwInputMode & ENABLE_QUICK_EDIT_MODE) { /* 快速编辑处于打开状态 */
        dwInputMode &= ~ENABLE_QUICK_EDIT_MODE; /* 关闭快速编辑 */
        std::puts("检测到快速编辑模式开启，尝试关闭快速编辑模式。");
        if (!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), dwInputMode)) {
            std::printf("设置控制台输入模式时出现错误。错误代码：%lu。\r\n", GetLastError());
            goto finish_config;
        }
    } else {
        std::puts("检测到快速编辑模式已经关闭。");
    }
    /* 设置控制台输出模式 */
    if (!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwOutputMode)) {
        std::printf("获取控制台输出模式失败。错误代码：%lu。\r\n", GetLastError());
        goto finish_config;
    }
    if (!(dwOutputMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) { /* 虚拟终端处理处于关闭状态 */
        dwOutputMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; /* 开启虚拟终端处理 */
        std::puts("检测到虚拟终端处理处于关闭状态，尝试开启虚拟终端处理。");
        if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwOutputMode)) {
            std::printf("设置控制台输出模式时出现错误。错误代码：%lu。\r\n", GetLastError());
            goto finish_config;
        }
    } else {
        std::puts("检测到虚拟终端处理已经开启。");
    }
finish_config:
    return ok;
}