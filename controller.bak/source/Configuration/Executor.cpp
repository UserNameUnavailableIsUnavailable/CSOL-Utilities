#include "Configuration.hpp"

namespace CSOL_Utilities
{

void from_json(const nlohmann::json& j, ExecutorConfiguration& ec)
{
    ec.command_file_path_ = j.at("Executor.CommandFilePath").get<std::string>();
}

void to_json(nlohmann::json& j, const ExecutorConfiguration& ec)
{
    j = {
        {
            "Executor.CommandFilePath",
            ec.command_file_path_
        }
    };
}

ExecutorConfiguration::ExecutorConfiguration(const nlohmann::json& json_obj)
{
    from_json(json_obj, *this);
}


} // CSOL_Utilities
