#pragma once

namespace CSOL_Utilities
{
std::vector<std::string> GetInputNames(Ort::Session &session);
std::vector<std::string> GetOutputNames(Ort::Session &session);
} // namespace CSOL_Utilities