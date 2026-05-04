#include "Configuration.hpp"
#include "Utilities.hpp"
#include "Exception.hpp"
#include <filesystem>
#include <format>
#include <stdexcept>
#include <vector>

namespace CSOL_Utilities
{
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

LocaleConfiguration::LocaleConfiguration(const nlohmann::json& json_obj)
{
    // get system locales
    system_locales_ = get_system_locales();
    // Locale.Directory: locale package directory
    // Locale.Name: locale name for translation, e.g. "zh-CN"

    // check if the locale name is supported by the system
    current_locale_ = json_obj.at("Locale.Name").get<std::string>();
    if (!system_locales_.contains(current_locale_))
    {
        throw std::runtime_error(std::format("Unsupported locale name: {}", current_locale_));
    }
    resources_dir_ = ConvertUtf8ToUtf16(json_obj.at("Locale.Directory").get<std::string>());
    std::string locale_file_name((current_locale_ + ".json"));
    auto package_path = resources_dir_ / std::u8string(locale_file_name.begin(), locale_file_name.end());
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
    translations_ = std::move(translations);
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

} // CSOL_Utilities
