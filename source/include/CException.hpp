#pragma once

#include "CSOL_Utilities.hpp"
#include <exception>

namespace CSOL_Utilities
{
    class CException : std::exception
    {
    public:
        CException(const char* detail);
        template<typename... VARARG> CException(const char* fmt, VARARG... args);
        inline const char* what() const noexcept override{ return m_Detail; };
    private:
        char m_Detail[256];
    };
};