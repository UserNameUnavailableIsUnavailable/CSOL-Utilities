#pragma once

#include "Utilities.hpp"

namespace CSOL_Utilities
{
	/// 控制台（单例），用于处理日志消息。
	class Console
	{
	public:
		static constexpr const char* COLOR_FORMAT = "\x1b[{}m";
		static constexpr const char* FOREGROUND_COLOR_GREEN = "\x1b[32m";
		static constexpr const char* FOREGROUND_COLOR_YELLOW = "\x1b[33m";
		static constexpr const char* FOREGROUND_COLOR_BLUE = "\x1b[34m";
		static constexpr const char* FOREGROUND_COLOR_RED = "\x1b[31m";
		static constexpr const char* FOREGROUND_COLOR_DEFAULT = "\x1b[39m";
		static constexpr const char* FOREGROUND_COLOR_WHITE = "\x1b[37m";
		static constexpr const char* FOREGROUND_COLOR_BRIGHT_GREEN = "\x1b[92m";
		static constexpr const char* FOREGROUND_COLOR_BRIGHT_YELLOW = "\x1b[93m";
		static constexpr const char* FOREGROUND_COLOR_BRIGHT_BLUE = "\x1b[94m";
		static constexpr const char* FOREGROUND_COLOR_BRIGHT_RED = "\x1b[101m";
		static constexpr const char* FOREGROUND_COLOR_BRIGHT_DEFAULT = "\x1b[39m";
		static constexpr const char* FOREGROUND_COLOR_BRIGHT_WHITE = "\x1b[37m";
		static constexpr const char* COLOR_DEFAULT = "\x1b[0m";
		static void Print(std::string_view s);
		static void Println(std::string_view s);
		static void Print(std::string_view ctrl_sequence, std::string_view s);
		static void Println(std::string_view ctrl_sequence, std::string_view s);
		template <typename... VA>
		static void Printf(std::string_view s, VA&&... va)
		{
			auto& console = GetInstance();
			auto str = std::vformat(s, std::make_format_args(std::forward<VA>(va)...));
			std::lock_guard lk(console.m_Lock);
			std::cout << str << std::flush;
		}
		template <typename... VA>
		static void Printfln(std::string_view s, VA&&... va)
		{
			auto& console = GetInstance();
			auto str = std::vformat(s, std::make_format_args(std::forward<VA>(va)...));
			std::lock_guard lk(console.m_Lock);
			std::cout << str << std::endl;
		}
		template <typename... VA>
		static void Printf(std::string_view ctrl_sequence, std::string_view s, VA&&... va)
		{
			auto& console = GetInstance();
			auto str = std::vformat(s, std::make_format_args(std::forward<VA>(va)...));
			std::lock_guard lk(console.m_Lock);
			std::cout << ctrl_sequence << str << std::flush;
		}
		template <typename... VA>
		static void Printfln(std::string_view ctrl_sequence, std::string_view s, VA&&... va)
		{
			auto& console = GetInstance();
			auto str = std::vformat(s, std::make_format_args(std::forward<VA>(va)...));
			std::lock_guard lk(console.m_Lock);
			std::cout << ctrl_sequence << str << std::endl;
		}
		static void Info(std::string_view s)
		{
			auto str = std::format("{}{}", Translate("Console::INFO_HEADER"), s);
			Console::Println(FOREGROUND_COLOR_GREEN + str + FOREGROUND_COLOR_DEFAULT);
		}
		static void Warn(std::string_view s)
		{
			auto str = std::format("{}{}", Translate("Console::WARN_HEADER"), s);
			Console::Println(FOREGROUND_COLOR_BRIGHT_YELLOW + str + FOREGROUND_COLOR_DEFAULT);
		}
		static void Error(std::string_view s)
		{
			auto str = std::format("{}{}", Translate("Console::ERROR_HEADER"), s);
			Console::Println(FOREGROUND_COLOR_BRIGHT_RED + str + FOREGROUND_COLOR_DEFAULT);
		}
		static void Debug(std::string_view s)
		{
			auto str = std::format("{}{}", Translate("Console::DEBUG_HEADER"), s);
			Console::Println(FOREGROUND_COLOR_BLUE + str + FOREGROUND_COLOR_DEFAULT);
		}
	private:
		Console();

		static Console& GetInstance()
		{
			static Console console;
			return console;
		}

		std::mutex m_Lock;
	};
} // namespace CSOL_Utilities
