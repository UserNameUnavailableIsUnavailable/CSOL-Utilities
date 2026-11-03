#include "ModelUtilities.hpp"

namespace CSOL_Utilities
{
std::vector<std::string> GetInputNames(Ort::Session &session)
{
    return session.GetInputNames();
}

std::vector<std::string> GetOutputNames(Ort::Session &session)
{
    return session.GetOutputNames();
}
} // namespace CSOL_Utilities