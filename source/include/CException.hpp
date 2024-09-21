#pragma once

#include "CSOL_Utilities.hpp"
#include <cstdio>
#include <exception>

namespace CSOL_Utilities
{
    class CException : std::exception
    {
    public:
        CException(const char* detail)
        {
            strcpy_s(m_Detail, detail);
        }
        template<typename... VARARG>
        CException(const char* fmt, VARARG... args)
        {
            std::snprintf(m_Detail, sizeof(m_Detail), fmt, args...);
        }
        const char* what() const noexcept override { return m_Detail; };
    private:
        char m_Detail[256];
    };
};