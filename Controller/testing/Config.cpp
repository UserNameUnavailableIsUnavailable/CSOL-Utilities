#include <gtest/gtest.h>

#include "Configuration.hpp"
#include "TestUtilities.hpp"

using namespace CSOL_Utilities;

TEST(ConfigurationManagerTest, LoadTest)
{
    auto json_file_path = GetBuildDirectory() / L"Controller.json";
    std::cout << "JSON config path: " << json_file_path << std::endl;
    std::ifstream ifs(json_file_path);
    ASSERT_TRUE(ifs.is_open());
    nlohmann::json json_obj = nlohmann::json::parse(ifs);
    ConfigurationManager cm(json_obj);
    auto gc = cm.Get<GameConfiguration>();
    ASSERT_TRUE(gc->IsGameLauncherAvailable());
    std::cout << std::format("Game Window Title: {}\n", gc->GetGameWindowTitle());
    std::cout << std::format("Is Launcher Available: {}\n", gc->IsGameLauncherAvailable());
    std::cout << std::format("Game Launch Command: {}\n", gc->GetGameLaunchCommand());
    auto lc = cm.Get<LocaleConfiguration>();
    std::cout << std::format("Current Locale: {}\n", lc->GetCurrentLocale());
    std::cout << "System Locales:";
    for (const auto& locale : lc->GetSystemLocales())
    {
        std::cout << " " << locale;
    }
    std::cout << '\n';
    std::cout << "Available Locales:";
    for (const auto& locale : lc->GetAvailableLocales())
    {
        std::cout << " " << locale;
    }
    std::cout << '\n';
    // std::cout << "Translations:\n";
    // for (const auto& [key, value] : lc->GetTranslation())
    // {
    //     std::cout << std::format("\t{}: {}\n", key, value);
    // }
    std::cout << std::endl;
    auto ic = cm.Get<IdleConfiguration>();
    std::cout << std::boolalpha;
    std::cout << "engine type: " << ic->GetEngineType() << '\n';
    std::cout << "model config path: " << ic->GetModelConfigPath() << '\n';
    std::cout << "login timeout: " << ic->GetLoginTimeout() << '\n';
    std::cout << "start game timeout: " << ic->GetStartGameRoomTimeout() << '\n';
    std::cout << "load map timeout: " << ic->GetLoadMapTimeout() << '\n';
    std::cout << "max in game time: " << ic->GetMaxInGameTime() << '\n';
    std::cout << "switch to default mode after reconnection: " << ic->SwitchToDefaultModeAfterReconnection() << '\n';
    std::cout << "restart game on loading timeout: " << ic->RestartGameOnLoadingTimeout() << '\n';
    std::cout << "suppress quick fullscreen: " << ic->SuppressQuickFullscreen() << '\n';
    std::cout << "suppress CSOBanner: " << ic->SuppressCSOBanner() << '\n';
}
