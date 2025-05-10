#pragma once

#include <Windows.h>
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <format>
#include <tlhelp32.h>
#include <functional>

namespace CSOL_Utilities
{
	std::wstring ConvertUtf8ToUtf16(const std::string& u8);
	std::string ConvertUtf16ToUtf8(const std::wstring& u16);
	BOOL IsRunningAsAdmin() noexcept;
	std::filesystem::path GetModulePath(uintptr_t hMod = 0ull);
	DWORD GetProcessIdByName(const std::wstring& process_name);
	void EnumProcesses(std::function<bool (const PROCESSENTRY32W& process_entry)> callback);
	void CaptureWindowAsBmp(HWND hWnd, std::vector<uint8_t>& buffer);
    void LoadLanguagePackage(std::unordered_map<std::string, std::string>& lang_pack); /* 根据 locale 加载语言包 */
	namespace Global
	{
		extern std::unordered_map<std::string, std::string> g_LanguagePackage; /* 语言包 */
	}
    template <typename... VA>
    std::string Translate(const std::string& key, VA&&... va)
    {
        if (Global::g_LanguagePackage.contains(key))
        {
			auto fmt = Global::g_LanguagePackage.at(key);
			return std::vformat(fmt, std::make_format_args(va...));
        }
        return key;
    }
}
