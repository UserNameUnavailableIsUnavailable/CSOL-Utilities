#include "Configuration.hpp"
#include <Windows.h>
#include "Utilities.hpp"
#include "Exception.hpp"
#include "nlohmann/json_fwd.hpp"
#include <filesystem>
#include <format>
#include <stdexcept>
#include <vector>

namespace CSOL_Utilities
{

// Helper: get locale names supported by the OS
static std::unordered_set<std::string> get_system_locales();
// Helper: resolve language json package to hashmap
static void resolve_language_json(std::unordered_map<std::string, std::string>& translations, nlohmann::json& translations_json_obj, std::string name_space);

void from_json(const nlohmann::json& j, LocaleConfiguration& lc)
{
    // get system locales
    lc.system_locales_ = get_system_locales();
    // Locale.Directory: locale package directory
    // Locale.Name: locale name for translation, e.g. "zh-CN"

    // check if the locale name is supported by the system
    lc.current_locale_ = j.at("Locale.Name").get<std::string>();
    if (!lc.system_locales_.contains(lc.current_locale_))
    {
        throw std::runtime_error(std::format("Unsupported locale name: {}", lc.current_locale_));
    }
    lc.resources_dir_ = ConvertUtf8ToUtf16(j.at("Locale.Directory").get<std::string>());
    std::string locale_file_name((lc.current_locale_ + ".json"));
    auto package_path = lc.resources_dir_ / std::u8string(locale_file_name.begin(), locale_file_name.end());
    if (!std::filesystem::is_regular_file(package_path))
    {
        throw std::runtime_error("Language package not found or is not an regular file.");
    }
    std::ifstream ifs(package_path, std::ios::binary | std::ios::in | std::ios::beg);
    if (!ifs.is_open())
    {
        throw std::runtime_error("Unable to open language package file.");
    }
    nlohmann::json translation_json_obj = nlohmann::json::parse(ifs);
    std::unordered_map<std::string, std::string> translations;
    resolve_language_json(translations, translation_json_obj, "");
    lc.translations_ = std::move(translations);

}

void to_json(nlohmann::json& j, const LocaleConfiguration& lc)
{
    j = {
        {
            "Locale.Name",
            lc.current_locale_
        },
        {
            "Locale.Directory",
            lc.resources_dir_
        }
    };
}

LocaleConfiguration::LocaleConfiguration(const nlohmann::json& json_obj)
{
    from_json(json_obj, *this);
}

std::vector<std::string> LocaleConfiguration::GetAvailableLocales() const
{
    std::vector<std::string> locales;
    locales.reserve(8);
    for (const auto& entry : std::filesystem::directory_iterator(resources_dir_))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            auto stem = entry.path().stem().u8string();
            std::string name = std::string(stem.begin(), stem.end());
            // skip invaslid locale names
            if (!system_locales_.contains(name)) continue;
            locales.push_back(name);
        }
    }
    return locales;
}

static std::unordered_set<std::string> get_system_locales()
{
    std::unordered_set<std::string> locales;
    auto enum_locales_proc = [](LPWSTR locale_name, DWORD flags, LPARAM param) -> BOOL {
        auto locales = reinterpret_cast<std::unordered_set<std::string>*>(param);
        std::wstring locale_name_w(locale_name);
        locales->insert(ConvertUtf16ToUtf8(locale_name_w));
        return TRUE;
    };

    BOOL ok = EnumSystemLocalesEx(enum_locales_proc, LOCALE_ALL, reinterpret_cast<LPARAM>(&locales), nullptr);
    if (!ok)
    {
        throw Exception(std::format("Failed to enumerate system locales. Win32 API error code: {}", GetLastError()));
    }
    return locales;
}

static void resolve_language_json(std::unordered_map<std::string, std::string>& translations, nlohmann::json& translations_json_obj, std::string name_space)
{
    for (auto &[key, value] : translations_json_obj.items())
    {
        std::string name = name_space.empty() ? key : name_space + "::" + key;
        if (value.is_object())
        {
            resolve_language_json(translations, value, name);
        }
        else if (value.is_string())
        {
            translations[name] = value.get<std::string>();
        }
        else
        {
            throw std::runtime_error("Invalid language package format.");
        }
    }
}

} // CSOL_Utilities
