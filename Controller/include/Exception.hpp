#pragma once

#include <exception>
#include <string>

namespace CSOL_Utilities
{
    class Exception : public std::exception
    {
    public:
        Exception(std::string s) : m_ErrorString(s) {}
        virtual const char* what() const noexcept override { return m_ErrorString.c_str(); }
    private:
        std::string m_ErrorString;
    };
}

#define DetailedQuickThrow(string) \
	throw CSOL_Utilities::Exception(CSOL_Utilities::Translate("Exception::DETAILED_QUICK_THROW_TEMPLATE", __FILE__, __LINE__, __func__, string))
