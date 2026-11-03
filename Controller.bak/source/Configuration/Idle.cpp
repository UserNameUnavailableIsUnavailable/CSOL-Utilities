#include "Configuration.hpp"

namespace CSOL_Utilities
{

void from_json(const nlohmann::json& j, IdleConfiguration& ic)
{
    ic.engine_type_ = j.value<std::string>("Idle.EngineType", ic.engine_type_);
    if (ic.engine_type_ != "Classifier")
    {
        throw std::runtime_error(std::format("Unsupported idle engine: {}.", ic.engine_type_));
    }
    ic.model_config_path_ = j.value("Idle.ModelConfigPath", ic.model_config_path_);
    ic.start_game_room_timeout_ = j.value("Idle.StartGameRoomTimeout", ic.start_game_room_timeout_);
    ic.login_timeout_ = j.value("Idle.LoginTimeout", ic.login_timeout_);
    ic.load_map_timeout_ = j.value("Idle.LoadMapTimeout", ic.load_map_timeout_);
    ic.max_in_game_time_ = j.value("Idle.MaxInGameTime", ic.max_in_game_time_);
    ic.switch_to_default_mode_after_reconnection = j.value("Idle.SwitchToDefaultModeAfterReconnection", ic.switch_to_default_mode_after_reconnection);
    ic.restart_game_on_loading_timeout_ = j.value("Idle.RestartGameOnLoadingTimeout", ic.restart_game_on_loading_timeout_);
    ic.suppress_quick_fullscreen_  = j.value("Idle.SuppressQuickFullscreen", ic.suppress_quick_fullscreen_);
    ic.suppress_cso_banner_ = j.value("Idle.SuppressCSOBanner", ic.suppress_cso_banner_);
}

void to_json(nlohmann::json& j, const IdleConfiguration& ic)
{
    j = {
    {
            "Idle.EngineType",
            ic.engine_type_
        },
        {
            "Idle.ModelConfigPath",
            ic.model_config_path_
        },
        {
            "Idle.StartGameRoomTimeout",
            ic.start_game_room_timeout_
        },
        {
            "Idle.LoginTimeout",
            ic.login_timeout_
        },
        {
            "Idle.LoadMapTimeout",
            ic.load_map_timeout_
        },
        {
            "Idle.MaxInGameTime",
            ic.max_in_game_time_
        },
        {
            "Idle.SwitchToDefaultModeAfterReconnection",
            ic.switch_to_default_mode_after_reconnection
        },
        {
            "Idle.RestartGameOnLoadingTimeout",
            ic.restart_game_on_loading_timeout_
        },
        {
            "Idle.SuppressQuickFullscreen",
            ic.suppress_quick_fullscreen_
        },
        {
            "Idle.SuppressCSOBanner",
            ic.suppress_cso_banner_
        }
    };
}

IdleConfiguration::IdleConfiguration(const nlohmann::json& j)
{
    from_json(j, *this);
}

} // namespace CSOL_Utilities

