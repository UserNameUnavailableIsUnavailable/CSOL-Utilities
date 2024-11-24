#pragma once

#include "ControlUnit.hpp"
#include <Windows.h>
#include <minwindef.h>

namespace CSOL_Utilities
{
template <typename MailBoxType>
class SlaveControlUnit : public ControlUnit<MailBoxType>
{
public:
    template <typename Callable, typename... VarArg>
    SlaveControlUnit(const ControlUnit<MailBoxType>& superior, CONTROL_UNIT_TYPE type, Callable&& c, VarArg&&... va);
    ~SlaveControlUnit() noexcept = default;
private:
    const ControlUnit<MailBoxType>& m_Superior; /* the slave CU sends messages to its superior, it is superior's job to dispatch corresponding commands to inferiors */
};



} /* namespace CSOL_Utilities */
