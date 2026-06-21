#include "ModelUtilities.hpp"

namespace CSOL_Utilities
{
std::vector<std::string> GetInputNames(Ort::Session &session)
{
    Ort::AllocatorWithDefaultOptions allocator;
    const size_t numInputNodes = session.GetInputCount();

    std::vector<std::string> input_names;
    input_names.reserve(numInputNodes);

    // iterate over all input nodes
    for (size_t i = 0; i < numInputNodes; i++)
    {
        char *input_name = session.GetInputName(i, allocator);
        if (input_name == nullptr)
        {
            input_names.emplace_back();
            continue;
        }
        input_names.emplace_back(input_name);
        allocator.Free(input_name);
    }
    return input_names;
}

std::vector<std::string> GetOutputNames(Ort::Session &session)
{
    Ort::AllocatorWithDefaultOptions allocator;
    const size_t numOutputNodes = session.GetOutputCount();

    std::vector<std::string> output_names;
    output_names.reserve(numOutputNodes);

    for (size_t i = 0; i < numOutputNodes; i++)
    {
        char *output_name = session.GetOutputName(i, allocator);
        if (output_name == nullptr)
        {
            output_names.emplace_back();
            continue;
        }
        output_names.emplace_back(output_name);
        allocator.Free(output_name);
    }
    return output_names;
}
} // namespace CSOL_Utilities