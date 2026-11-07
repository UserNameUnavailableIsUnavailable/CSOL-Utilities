#include "Configuration.hpp"

namespace CSOL_Utilities
{

void ConfigurationManager::Load(const nlohmann::json& json_obj)
{
    game_cfg_ = std::make_shared<GameConfiguration>(json_obj);
    locale_cfg_ = std::make_shared<LocaleConfiguration>(json_obj);
    idle_cfg_ = std::make_shared<IdleConfiguration>(json_obj);

    for (auto cb : reload_callbacks_)
    {
        cb();
    }
}

}
