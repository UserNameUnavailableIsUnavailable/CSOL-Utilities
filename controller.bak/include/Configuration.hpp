#pragma once

#include "nlohmann/json_fwd.hpp"
#ifndef CONTROLLER_VERSION
#define CONTROLLER_VERSION "1.0.0"
#endif

namespace CSOL_Utilities
{

// For each configuration type, we need to implement friend functions to_json and from_json,
// which allows nlohmann::json to serialize and deserialize.

class GameConfiguration
{
    friend void to_json(nlohmann::json& j, const GameConfiguration& gc);
    friend void from_json(const nlohmann::json& j, GameConfiguration& gc);
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
    friend void to_json(nlohmann::json& j, const LocaleConfiguration& lc);
    friend void from_json(const nlohmann::json& j, LocaleConfiguration& lc);
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
    friend void from_json(const nlohmann::json& j, IdleConfiguration& ic);
    friend void to_json(nlohmann::json& j, const IdleConfiguration& ic);
public:
    // Currently, only Classifer is allowed
    const std::string& GetEngineType() const noexcept
    {
        return engine_type_;
    }
    const std::string& GetModelConfigPath() const noexcept
    {
        return model_config_path_;
    }
    std::int32_t GetStartGameRoomTimeout() const noexcept
    {
        return start_game_room_timeout_;
    }
    std::int32_t GetLoginTimeout() const noexcept
    {
        return login_timeout_;
    }
    std::int32_t GetLoadMapTimeout() const noexcept
    {
        return load_map_timeout_;
    }
    std::int32_t GetMaxInGameTime() const noexcept
    {
        return max_in_game_time_;
    }
    bool SwitchToDefaultModeAfterReconnection() const noexcept
    {
        return switch_to_default_mode_after_reconnection;
    }
    bool RestartGameOnLoadingTimeout() const noexcept
    {
        return restart_game_on_loading_timeout_;
    }
    bool SuppressQuickFullscreen() const noexcept
    {
        return suppress_quick_fullscreen_;
    }
    bool SuppressCSOBanner() const noexcept
    {
        return suppress_cso_banner_;
    }

    IdleConfiguration(const nlohmann::json& j);
    IdleConfiguration() = default;
    ~IdleConfiguration() noexcept = default;

private:
    std::string engine_type_ = "Classfier";
    std::string model_config_path_ = "models/Classifier/ResNet/CSOL-Utilities-ResNet18-800x600.json";
    std::int32_t start_game_room_timeout_ = 5 * 60;
    std::int32_t login_timeout_ = 5 * 60;
    std::int32_t load_map_timeout_ = 10 * 60;
    std::int32_t max_in_game_time_ = UINT_MAX;
    bool switch_to_default_mode_after_reconnection = true;
    bool restart_game_on_loading_timeout_ = true;
    bool suppress_quick_fullscreen_ = true;
    bool suppress_cso_banner_ = true;
};

class ExecutorConfiguration
{
    friend void from_json(const nlohmann::json& j, ExecutorConfiguration& ec);
    friend void to_json(nlohmann::json& j, const ExecutorConfiguration& ec);
public:
    ExecutorConfiguration() = default;
    ExecutorConfiguration(const nlohmann::json& json_obj);
    ~ExecutorConfiguration() noexcept = default;
    const std::string& GetCommandFilePath() const noexcept
    {
        return command_file_path_;
    }
    const std::string& GetAgentProcessName() const noexcept
    {
        return lghub_agent_process_name_;
    }
private:
    std::string command_file_path_;
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
    std::shared_ptr<IdleConfiguration> idle_cfg_;
    std::vector<std::function<void ()>> reload_callbacks_;

DEFINE_CONFIGURATION_ACCESSOR(locale_cfg_);
DEFINE_CONFIGURATION_ACCESSOR(game_cfg_);
DEFINE_CONFIGURATION_ACCESSOR(idle_cfg_);

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
