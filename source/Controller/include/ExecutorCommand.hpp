#pragma once

#include <mutex>
#include "CSOL_Utilities.hpp"


namespace CSOL_Utilities
{
class ExecutorCommand
{
private:
    static std::mutex s_Mutex;
    static std::uint64_t s_Counter;
    std::uint64_t m_Id = 0;
    EXECUTOR_COMMAND m_Cmd = EXECUTOR_COMMAND::CMD_NOP;
    std::time_t m_CmdTimepoint = 0;
    std::time_t m_LastAllocationTimepoint = 0;
    bool m_bRepeatable = false; /* 是否为可重复类型 */
    const std::time_t DURATION; /* ID 持续时间，该段时间内不会分配新的 ID，仍保留旧 ID */ 
public:
    explicit ExecutorCommand(std::time_t duration, bool repeatable = false) :
        DURATION(duration), m_bRepeatable(repeatable) {}
    ~ExecutorCommand() noexcept = default;
    auto GetId() const
    {
        return m_Id;
    }
    auto GetCmd() const
    {
        return m_Cmd;
    }
    auto GetCmdTimepoint() const
    {
        return m_CmdTimepoint;
    }
    auto IsRepeatable() const
    {
        return m_bRepeatable;
    }
    void ToggleRepeatable(bool repeatable)
    {
        m_bRepeatable = repeatable;
    }
    bool Set(EXECUTOR_COMMAND cmd, std::time_t cmd_timepoint);
};
}
