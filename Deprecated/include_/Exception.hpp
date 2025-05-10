#pragma once

#include <cstdio>
#include <cstring>
#include <exception>
#include <format>
#include <sal.h>
#include <string>

namespace CSOL_Utilities
{
	class Exception : public std::exception
	{
	public:
		// 构造异常对象
		explicit Exception(std::string&& s) : m_detail(s)
		{
		}

		const char* what() const noexcept override
		{
			return m_detail.c_str();
		};

	private:
		std::string m_detail;
	};
}; // namespace CSOL_Utilities
