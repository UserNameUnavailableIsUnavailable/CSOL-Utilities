#include "Configuration.hpp"

namespace CSOL_Utilities
{

void to_json(nlohmann::json& j, const GameConfiguration& gc)
{
    j = nlohmann::json{
        { "Game.IsLauncherAvailable", gc.is_launcher_available_ },
        { "Game.RootDirectory", gc.game_root_dir_ },
        { "Game.LauncherExecutablePath", gc.launcher_executable_path_ },
        { "Game.LauncherArguments", gc.launcher_arguments_ },
        { "Game.WindowTitle", gc.game_window_title_ }
    };
}

void from_json(const nlohmann::json& j, GameConfiguration& gc)
{
    if (!j.is_object())
    {
        throw std::runtime_error("Invalid JSON object for game configuration");
    }
    gc.is_launcher_available_ = j.value("Game.IsLauncherAvailable", false);
    gc.game_root_dir_ = j.value("Game.RootDirectory", "");
    if (gc.is_launcher_available_)
    {
        gc.launcher_executable_path_ = j.value("Game.LauncherExecutablePath", "");
        gc.launcher_arguments_ = j.value("Game.LauncherArguments", std::vector<std::string>{});
    }
    gc.game_window_title_ = j.value("Game.WindowTitle", gc.game_window_title_);
}

GameConfiguration::GameConfiguration(const nlohmann::json& json_obj)
{
    from_json(json_obj, *this);
}

std::string GameConfiguration::GetGameLaunchCommand() const
{
    if (!is_launcher_available_)
    {
        throw std::runtime_error("Launcher is not available");
    }
    std::stringstream ss;
    for (const auto& arg : launcher_arguments_)
    {
        ss << " " << arg;
    }
    return std::format("\"{}\"{}", launcher_executable_path_, ss.str());
}

} // namespace CSOL_Utilities
