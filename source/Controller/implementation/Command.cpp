#include <chrono>
#include "Command.hpp"

namespace CSOL_Utilities
{
    void Command::nop()
    {
        std::lock_guard lock(m_mtx);
        m_cmd = COMMAND::CMD_NOP;
        m_repeatable = false;
    }
    void Command::get(std::string& s)
    {
        std::lock_guard lock(m_mtx);
        static constexpr const char* command_fmt = 
            "CmdId = %lld\n"
            "CmdName = %s\n"
            "CmdTimepoint = %lld\n"
            "CmdRepeatable = %s\n";
        if (m_cmd != COMMAND::CMD_NOP)
        {
            m_id++;
        }
        while (true)
        {
            auto required = snprintf(s.data(), s.size(), command_fmt, m_id, query_command_string(m_cmd),  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()), m_repeatable ? "true" : "false");
            if (required >= s.size())
            {
                s.resize(required + required / 2, '\0'); /* 扩容 1.5 倍 */
            }
            else
            {
                break;
            }
        }
    }
    void Command::set(COMMAND cmd, bool repeatable)
    {
        std::lock_guard lock(m_mtx);
        m_cmd = cmd;
        m_repeatable = repeatable;
    }
}