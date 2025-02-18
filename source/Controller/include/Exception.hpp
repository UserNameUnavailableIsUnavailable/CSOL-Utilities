#pragma once

#include <cstdio>
#include <exception>
#include <sal.h>
#include <string>
#include <CSOL_Utilities.hpp>

namespace CSOL_Utilities
{
class Exception : public std::exception
{
public:
    Exception(const char *detail) : m_Level(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_ERROR)
    {
        strcpy_s(m_Detail, detail);
    }
    template <typename... VARARG> Exception(_Printf_format_string_ const char* fmt, VARARG... args) :
        m_Level(CSOL_UTILITIES_MESSAGE_LEVEL::CUML_ERROR)
    {
        std::snprintf(m_Detail, sizeof(m_Detail), fmt, args...);
    }
    template <typename... VARARG> Exception(CSOL_UTILITIES_MESSAGE_LEVEL level, _Printf_format_string_ const char *fmt, VARARG... args) :
        m_Level(level)
    {
        std::snprintf(m_Detail, sizeof(m_Detail), fmt, args...);
    }
	  CSOL_UTILITIES_MESSAGE_LEVEL GetLevel() const { return m_Level; }
    const char *what() const noexcept override
    {
        return m_Detail;
    };
    private:
      CSOL_UTILITIES_MESSAGE_LEVEL m_Level;
	char m_Detail[512];
};
}; // namespace CSOL_Utilities