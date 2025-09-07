#pragma once

namespace CSOL_Utilities
{
    std::vector<Ort::AllocatedStringPtr> GetInputNames(Ort::Session& session);
    std::vector<Ort::AllocatedStringPtr> GetOutputNames(Ort::Session& session);
}