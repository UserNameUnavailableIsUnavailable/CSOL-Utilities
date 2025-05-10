#include "Console.hpp"
#include <ctime>
#include <errhandlingapi.h>
#include <format>
#include <mutex>
#include "CSOL_Utilities.hpp"

namespace CSOL_Utilities
{
	std::mutex Console::m_Mutex{};

	bool Console::Configure(CONTROL_EVENT_HANDLER on_destroy) noexcept
	{
		bool ok = true;
		/* 先设置代码页和 locale 以便处理中文输入 */
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		std::setlocale(LC_ALL, ".UTF-8");
		std::cout << Opt::g_LanguagePackage.at("Console").at("CODE_PAGE_CHANGED_TO_UTF-8").get<std::string>()
				  << std::endl;
		/* 控制台信号处理 */
		if (!SetConsoleCtrlHandler(on_destroy, TRUE))
		{
			auto error_code = GetLastError();
			auto content = std::vformat(
				Opt::g_LanguagePackage.at("Console").at("FAILED_TO_REGISTER_CTRL_EVENT_HANDLER@1").get<std::string>(),
				std::make_format_args(error_code));
			std::cout << content << std::endl;
			ok = false;
			goto finish_config;
		}

		DWORD dwInputMode, dwOutputMode;
		/* 设置控制台输入模式 */
		if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dwInputMode))
		{
			auto error_code = GetLastError();
			auto content =
				std::vformat(Opt::g_LanguagePackage.at("Console").at("FAILED_TO_GET_INPUT_MODE@1").get<std::string>(),
							 std::make_format_args(error_code));
			std::cout << content << std::endl;
			goto finish_config;
		}
		if (dwInputMode & ENABLE_QUICK_EDIT_MODE)
		{ /* 快速编辑处于打开状态 */
			dwInputMode &= ~ENABLE_QUICK_EDIT_MODE; /* 关闭快速编辑 */
			if (!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), dwInputMode))
			{
				auto error_code = GetLastError();
				auto content = std::vformat(
					Opt::g_LanguagePackage.at("Console").at("FAILED_TO_SET_INPUT_MODE@1").get<std::string>(),
					std::make_format_args(error_code));
				std::cout << content << std::endl;
				goto finish_config;
			}
		}
		/* 设置控制台输出模式 */
		if (!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwOutputMode))
		{
			auto error_code = GetLastError();
			auto content =
				std::vformat(Opt::g_LanguagePackage.at("Console").at("FAILED_TO_GET_OUTPUT_MODE@1").get<std::string>(),
							 std::make_format_args(error_code));
			std::cout << content << std::endl;
			goto finish_config;
		}
		if (!(dwOutputMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
		{ /* 虚拟终端处理处于关闭状态 */
			dwOutputMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; /* 开启虚拟终端处理 */
			if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwOutputMode))
			{
				auto error_code = GetLastError();
				auto content = std::vformat(
					Opt::g_LanguagePackage.at("Console").at("FAILED_TO_SET_OUTPUT_MODE@1").get<std::string>(),
					std::make_format_args(error_code));
				std::cout << content << std::endl;
				goto finish_config;
			}
		}
	finish_config:
		return ok;
	}
} // namespace CSOL_Utilities
