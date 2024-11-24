#pragma once

#include "ControlUnit.hpp"
#include "InGameStateAnalyzer.hpp"
#include "SlaveControlUnit.hpp"
#include "MailboxTypes.hpp"
#include <filesystem>

namespace CSOL_Utilities
{

struct AnalyzerControlMessage
{
    bool bEnableExtendedMode;
};

class InGameStateAnalyzer : public SlaveControlUnit<AnalyzerOption>
{
friend void AnalyzeInGameState(InGameStateAnalyzer&);
public:
    template <typename MailBoxType>
    InGameStateAnalyzer(ControlUnit<MailBoxType>& superior, std::filesystem::path p);
    ~InGameStateAnalyzer() noexcept = default;
private:
};
void AnalyzeInGameState(InGameStateAnalyzer&);
}