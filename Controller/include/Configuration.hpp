#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#ifndef CONTROLLER_VERSION
#define CONTROLLER_VERSION "1.0.0"
#endif

namespace CSOL_Utilities
{
class GameConfiguration
{
public:
    GameConfiguration(const nlohmann::json& json_obj);
    ~GameConfiguration() noexcept = default;
    bool IsGameLauncherAvailable() const noexcept { return is_launcher_available_; }
    std::string GetGameWindowTitle() const { return game_window_title_; }
    std::string GetGameExecutablePath() const;
    std::string GetGameLaunchCommand() const;
private:
    std::string game_root_dir_;
    std::string game_window_title_ = "Counter-Strike Online";
    bool is_launcher_available_ = false;
    std::string launcher_executable_path_;
    std::vector<std::string> launcher_arguments_;
};

class LocaleConfiguration
{
public:
    LocaleConfiguration(const nlohmann::json& json_obj);
    ~LocaleConfiguration() noexcept = default;
    const std::unordered_set<std::string>& GetSystemLocales() const noexcept
    {
        return system_locales_;
    }
    std::vector<std::string> GetAvailableLocales() const;
    const std::string& GetCurrentLocale() const noexcept
    {
        return current_locale_;
    }
    const std::unordered_map<std::string, std::string>& GetTranslation() const noexcept
    {
        return translations_;
    }
private:
    std::filesystem::path resources_dir_;
    std::unordered_set<std::string> system_locales_;
    std::string current_locale_;
    std::unordered_map<std::string, std::string> translations_;
};

struct IdleConfiguration
{
    std::string engine_type = "Classfier";
    std::uint32_t start_game_room_timeout = 5 * 60;
    std::uint32_t login_timeout = 5 * 60;
    std::uint32_t load_map_timeout = 10 * 60;
    std::uint32_t max_in_game_time = UINT_MAX;
    bool default_idle_after_reconnection = true;
    bool restart_game_on_loading_timeout = true;
    bool allow_quick_full_screen = false;
    bool suppress_cso_banner = true;
    std::string ocr_detector_json_path;
    std::string ocr_recognizer_json_path;
    std::string ocr_keywords_json_path;
    std::string classifier_model_json_path;
};

struct ExecutorConfiguration
{
    std::filesystem::path command_file_path_;
    std::string lghub_agent_process_name_;
};

class ConfigurationManager;

// ConfigurationManager is responsible for loading and providing access to the configuration data
// Once a configuration is loaded, that configuration is immutable and shared across the application, therefore ensuring thread safety without the need for locks.
// The configuration can be reloaded by calling Load() with new configuration data, which will atomically replace the old configuration with the new one. Those threads referencing the old configuration will continue to use it until they are done.
// NOTE: This module has some limitations in that it does not support partial updates to the configuration. When Load() is called, the entire configuration is replaced with the new one, and all callbacks registered will be triggered.
class ConfigurationManager
{
template <typename T>
struct Accessor;

#define DEFINE_CONFIGURATION_ACCESSOR(member) \
template<>\
struct Accessor<decltype(ConfigurationManager::member)> {\
    static const auto& Get(const ConfigurationManager& cm) { return cm.member; }\
}
private:
    std::shared_ptr<LocaleConfiguration> locale_cfg_;
    std::shared_ptr<GameConfiguration> game_cfg_;
    std::vector<std::function<void ()>> reload_callbacks_;

DEFINE_CONFIGURATION_ACCESSOR(locale_cfg_);
DEFINE_CONFIGURATION_ACCESSOR(game_cfg_);

public:
    ConfigurationManager(const nlohmann::json& json_obj)
    {
        Load(json_obj);
    }
    void Load(const nlohmann::json& json_obj);
    void RegisterReloadCallback(std::function<void ()> callback)
    {
        reload_callbacks_.push_back(std::move(callback));
    }
    template <typename ConfigurationType>
    auto Get()
    {
        return Accessor<std::shared_ptr<ConfigurationType>>::Get(*this);
    }
    template <typename ConfigurationType>
    auto Get() const
    {
        return Accessor<std::shared_ptr<ConfigurationType>>::Get(*this);
    }
};


} // namespace CSOL_Utilities
