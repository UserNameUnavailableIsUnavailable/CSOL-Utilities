#pragma once

#include "CSOL_Utilities.hpp"
#include <Windows.h>
#include <cstdio>
#include <mutex>
#include <iostream>
#include <random>

namespace CSOL_Utilities
{
    class CConsole
    {
    public:
        static bool ConfigureConsole(BOOL(*on_destroy_callback)(DWORD dwCtrlType)) noexcept;
        static int Log(CONSOLE_LOG_LEVEL level, const char* string) noexcept;
        template <typename... ARG> static int Log(CONSOLE_LOG_LEVEL level, const char* fmt, const ARG&... args) noexcept
        {
            std::lock_guard<std::mutex> lock_guard(m_Mutex);
            int ret;
            if (level == CONSOLE_LOG_LEVEL::CLL_MESSAGE) {
                std::printf("【消息】");
                std::printf(fmt, args...);
                std::printf("\r\n");
            } else if (level == CONSOLE_LOG_LEVEL::CLL_ERROR) {
                std::printf("\x1b[91m【错误】"); /* 红色字体打印错误 */
                ret = std::printf(fmt, args...);
                std::printf("\x1b[39m\r\n");
            } else if (level == CONSOLE_LOG_LEVEL::CLL_WARNING) {
                std::printf("\x1b[93m【警告】"); /* 黄色字体打印警告 */
                ret = std::printf(fmt, args...);
                std::printf("\x1b[39m\r\n");
            }
            std::fflush(stdout);
            return ret;
        };
    private:
        CConsole() = delete;
        ~CConsole() = delete;
        static BOOL(*m_OnDestroyCallback)(DWORD dwCtrlType);
        static std::mutex m_Mutex;
    };
}