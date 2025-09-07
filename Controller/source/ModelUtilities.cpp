#include "ModelUtilities.hpp"

namespace CSOL_Utilities
{
std::vector<Ort::AllocatedStringPtr> GetInputNames(Ort::Session& session)
{
    Ort::AllocatorWithDefaultOptions allocator;
    const size_t numInputNodes = session.GetInputCount();

    std::vector<Ort::AllocatedStringPtr> inputNamesPtr;
    inputNamesPtr.reserve(numInputNodes);
    std::vector<int64_t> input_node_dims;

    // iterate over all input nodes
    for (size_t i = 0; i < numInputNodes; i++)
    {
        auto inputName = session.GetInputNameAllocated(i, allocator);
        inputNamesPtr.push_back(std::move(inputName));
    }
    return inputNamesPtr;
}

std::vector<Ort::AllocatedStringPtr> GetOutputNames(Ort::Session& session)
{
    Ort::AllocatorWithDefaultOptions allocator;
    const size_t numOutputNodes = session.GetOutputCount();

    std::vector<Ort::AllocatedStringPtr> outputNamesPtr;
    outputNamesPtr.reserve(numOutputNodes);
    std::vector<int64_t> output_node_dims;

    for (size_t i = 0; i < numOutputNodes; i++)
    {
        auto outputName = session.GetOutputNameAllocated(i, allocator);
        outputNamesPtr.push_back(std::move(outputName));
    }
    return outputNamesPtr;
}
}