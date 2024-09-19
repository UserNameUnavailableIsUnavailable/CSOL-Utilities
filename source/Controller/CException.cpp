#include "CException.hpp"
#include <cstdio>
#include <cstring>

namespace CSOL_Utilities
{
    CException::CException(const char* detail)
    {
        strcpy_s(m_Detail, detail);
    }
    template<typename... VARARG>
    CException::CException(const char* fmt, VARARG... args)
    {
        std::snprintf(m_Detail, sizeof(m_Detail), fmt, args...);
    }
}