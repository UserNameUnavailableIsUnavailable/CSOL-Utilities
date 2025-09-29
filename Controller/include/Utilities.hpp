#pragma once

namespace CSOL_Utilities
{
namespace Global
{
extern std::unordered_map<std::string, std::string> g_LanguagePackage; /* 语言包 */
}

std::wstring ConvertUtf8ToUtf16(const std::string &u8);
std::string ConvertUtf16ToUtf8(const std::wstring &u16);
BOOL IsRunningAsAdmin() noexcept;
std::filesystem::path GetProcessImagePath(uintptr_t hMod = 0);
DWORD GetProcessIdByName(const std::wstring &process_name);
void EnumerateProcesses(std::function<bool(const PROCESSENTRY32W &process_entry)> callback);
void CaptureWindowAsBmp(HWND hWnd, std::vector<uint8_t> &buffer);
void LoadLanguagePackage(std::unordered_map<std::string, std::string> &lang_pack); /* 根据 locale 加载语言包 */
void RemoveWindowBorder(HWND hWnd) noexcept;
void CenterWindowClientArea(HWND hWnd) noexcept;
void CenterWindow(HWND hWnd) noexcept;

template <typename... VA> class TranslationKey
{
  public:
    template <class StringType>
        requires std::convertible_to<StringType, std::string_view>
    consteval TranslationKey(const StringType &s) : m_StringView(s)
    {
        auto idx = m_StringView.find('@');
        if (idx == m_StringView.npos)
        {
            if (sizeof...(VA) != 0)
            {
                throw "Expected no arguments";
            }
        }
        else
        {
            std::size_t params_cnt{0};
            for (auto i = idx + 1; m_StringView.begin() + i != m_StringView.end(); i++)
            {
                auto digit = m_StringView.at(i);
                if ('0' <= digit && digit <= '9')
                {
                    params_cnt = params_cnt * 10 + digit - '0';
                }
                else
                {
                    throw "Mismatching number of parameters";
                }
            }
            if (sizeof...(VA) != params_cnt)
            {
                throw "Invalid translate key format";
            }
        }
    }
    constexpr std::string_view GetStringView() const
    {
        return m_StringView;
    }

  private:
    std::string_view m_StringView;
};

template <typename... VA> std::string Translate(TranslationKey<std::type_identity_t<VA>...> translation_key, VA &&...va)
{
    std::string translation_unit_key(translation_key.GetStringView());
    if (Global::g_LanguagePackage.contains(translation_unit_key))
    {
        std::string fmt = Global::g_LanguagePackage.at(translation_unit_key);
        return std::vformat(fmt, std::make_format_args(va...));
    }
    else
    {
        std::stringstream ss;
        ss << translation_unit_key << "(";
        bool first = true;
        ((ss << (first ? "" : ", ") << va, first = false), ...);
        ss << ")";
        return ss.str();
    }
}

bool SafeTerminateProcess(HANDLE hProcess, DWORD dwMilliseconds) noexcept;
} // namespace CSOL_Utilities
