#pragma once

#include "ControlUnit.hpp"

namespace CSOL_Utilities
{
template <typename MailBoxType>
class CMasterControlUnit : public ControlUnit<MailBoxType>
{
public:
    template <typename Callable, typename... VarArg>
    CMasterControlUnit(CONTROL_UNIT_TYPE type, Callable&& c, VarArg... va);
    ~CMasterControlUnit() noexcept = default;
private:
};
} /* namespace CSOL_Utilities */
