#pragma once

#include "CSOL_Utilities.hpp"
#include <Windows.h>
#include <cstdio>
#include <mutex>
#include <sal.h>

using DESTROY_CALLBACK = BOOL (*)(DWORD);

namespace CSOL_Utilities
{
class CConsole
{
  public:
    static bool Configure(DESTROY_CALLBACK on_destroy) noexcept;
    static void Log(CONSOLE_LOG_LEVEL level, std::string s) noexcept;
    template <typename... VARARG>
    static void Log(CONSOLE_LOG_LEVEL level, _Printf_format_string_ const char *fmt, const VARARG &...args) noexcept
    {
        std::lock_guard<std::mutex> lock_guard(m_Mutex);
        std::time_t current_time =
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        std::tm current_tm;
        localtime_s(&current_tm, &current_time);
        char current_time_string[32];
        strftime(current_time_string, sizeof(current_time_string), "%Y/%m/%d %H:%M:%S", &current_tm);
        if (level == CONSOLE_LOG_LEVEL::CLL_MESSAGE)
        {
            std::printf("\x1b[92m%s【消息】", current_time_string);
            std::printf(_Printf_format_string_ fmt, args...);
            std::printf("\x1b[39m"
                        "\r\n");
        }
        else if (level == CONSOLE_LOG_LEVEL::CLL_ERROR)
        {
            std::printf("\x1b[91m%s【错误】", current_time_string); /* 红色字体打印错误 */
            std::printf(fmt, args...);
            std::printf("\x1b[39m"
                        "\r\n");
        }
        else if (level == CONSOLE_LOG_LEVEL::CLL_WARNING)
        {
            std::printf("\x1b[93m%s【警告】", current_time_string); /* 黄色字体打印警告 */
            std::printf(_Printf_format_string_ fmt, args...);
            std::printf("\x1b[39m"
                        "\r\n");
        }
        std::fflush(stdout);
    };

  private:
    CConsole() = delete;
    ~CConsole() = delete;
    static std::mutex m_Mutex;
};
} // namespace CSOL_Utilities