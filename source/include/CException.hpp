#pragma once

#include <string>
#include <cstdio>
#include <sal.h>
#include <exception>

namespace CSOL_Utilities
{
    class CException : std::exception
    {
    public:
        CException(const char* detail)
        {
            std::strcpy(m_Detail, detail);
        }
        template<typename... VARARG>
        CException(_Printf_format_string_ const char* fmt, VARARG... args)
        {
            std::snprintf(m_Detail, sizeof(m_Detail), fmt, args...);
        }
        const char* what() const noexcept override { return m_Detail; };
    private:
        char m_Detail[512];
    };
};