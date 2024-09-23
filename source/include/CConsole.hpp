#pragma once

#include "CSOL_Utilities.hpp"
#include <Windows.h>
#include <cstdio>
#include <mutex>
#include <sal.h>

using DESTROY_CALLBACK = BOOL(*)(DWORD);

namespace CSOL_Utilities
{
    class CConsole
    {
    public:
        static bool Configure(DESTROY_CALLBACK on_destroy) noexcept;
        static void Log(CONSOLE_LOG_LEVEL level, std::string s) noexcept;
        template <typename... VARARG>
        static void Log(CONSOLE_LOG_LEVEL level, _Printf_format_string_ const char* fmt, const VARARG&... args) noexcept
        {
            std::lock_guard<std::mutex> lock_guard(m_Mutex);
            if (level == CONSOLE_LOG_LEVEL::CLL_MESSAGE) {
                std::printf("\x1b[92m【消息】");
                std::printf(fmt, args...);
                std::printf("\r\n");
            } else if (level == CONSOLE_LOG_LEVEL::CLL_ERROR) {
                std::printf("\x1b[91m【错误】"); /* 红色字体打印错误 */
                std::printf(fmt, args...);
                std::printf("\r\n");
            } else if (level == CONSOLE_LOG_LEVEL::CLL_WARNING) {
                std::printf("\x1b[93m【警告】"); /* 黄色字体打印警告 */
                std::printf(fmt, args...);
                std::printf("\r\n");
            }
            std::fflush(stdout);
        };
    private:
        CConsole() = delete;
        ~CConsole() = delete;
        static std::mutex m_Mutex;
    };
}