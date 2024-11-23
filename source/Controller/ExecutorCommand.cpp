#include "ExecutorCommand.hpp"
#include <cstdint>

namespace CSOL_Utilities
{
std::mutex ExecutorCommand::s_Mutex;
std::uint64_t ExecutorCommand::s_Counter;

bool ExecutorCommand::Set(EXECUTOR_COMMAND cmd, std::time_t cmd_timepoint)
{
    auto current_time = std::time(nullptr);
    if (current_time - m_LastAllocationTimepoint < DURATION)
    {
        return false;
    }
    std::lock_guard<std::mutex> lock(s_Mutex);
    m_Id = ++s_Counter;
    m_Cmd = cmd;
    m_CmdTimepoint = cmd_timepoint;
    return true;
};
}