#pragma once

#include <sal.h>
#include <exception>
#include <cstdio>

namespace CSOL_Utilities
{
    class Exception : public std::exception
    {
    public:
        // 构造异常对象
        explicit Exception(const char *detail)
        {
            strcpy_s(m_detail, detail);
        }

        // 用格式化字符串的方式构造异常对象
        template <typename... VARARG>
        explicit Exception(_Printf_format_string_ const char* fmt, VARARG... args)
        {
            std::snprintf(m_detail, sizeof(m_detail), fmt, args...);
        }


        const char *what() const noexcept override
        {
            return m_detail;
        };
        private:
        char m_detail[512];
    };
}; // namespace CSOL_Utilities