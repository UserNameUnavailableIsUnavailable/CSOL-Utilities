#include "Configuration.hpp"

namespace CSOL_Utilities
{

GameConfiguration::GameConfiguration(const nlohmann::json& json_obj)
{
    if (!json_obj.is_object())
    {
        throw std::runtime_error("Invalid JSON object for game configuration");
    }
    is_launcher_available_ = json_obj.value("Game.IsLauncherAvailable", false);
    game_root_dir_ = json_obj.value("Game.RootDirectory", "");
    if (is_launcher_available_)
    {
        launcher_executable_path_ = json_obj.value("Game.LauncherExecutablePath", "");
        launcher_arguments_ = json_obj.value("Game.LauncherArguments", std::vector<std::string>{});
    }
    game_window_title_ = json_obj.value("Game.WindowTitle", game_window_title_);
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
