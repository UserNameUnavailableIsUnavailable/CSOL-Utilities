#pragma once

#include "pch.hpp"

namespace CSOL_Utilities
{
	namespace Global
	{
		extern std::unordered_map<std::string, std::string> g_LanguagePackage; /* 语言包 */
	}

	std::wstring ConvertUtf8ToUtf16(const std::string& u8);
	std::string ConvertUtf16ToUtf8(const std::wstring& u16);
	BOOL IsRunningAsAdmin() noexcept;
	std::filesystem::path GetModulePath(uintptr_t hMod = 0ull);
	DWORD GetProcessIdByName(const std::wstring& process_name);
	void EnumProcesses(std::function<bool (const PROCESSENTRY32W& process_entry)> callback);
	void CaptureWindowAsBmp(HWND hWnd, std::vector<uint8_t>& buffer);
    void LoadLanguagePackage(std::unordered_map<std::string, std::string>& lang_pack); /* 根据 locale 加载语言包 */
	void RemoveWindowBorder(HWND hWnd) noexcept;
	void CenterWindow(HWND hWnd) noexcept;
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
	bool SafeTerminateProcess(HANDLE hProcess, DWORD dwMilliseconds) noexcept;
}
