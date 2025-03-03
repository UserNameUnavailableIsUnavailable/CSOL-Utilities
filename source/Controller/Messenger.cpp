#include "Messenger.hpp"
#include <filesystem>
#include <mutex>
namespace CSOL_Utilities
{
Messenger::Messenger(std::filesystem::path file)
    : m_CommandFile(file), m_FileStream(m_CommandFile, std::ios::out | std::ios::binary),
      m_CommandString(QueryCommandString(EXECUTOR_COMMAND::CMD_NOP)), m_Mutex(), m_Buffer(new char[512])
{
}

Messenger::~Messenger() noexcept
{
    DispatchNOP();
    delete[] m_Buffer;
}

void Messenger::DispatchNOP() noexcept
{
    m_FileStream.seekp(0);
    m_FileStream << "CmdId = 0\n"
                 << "CmdType = " << QueryCommandString(EXECUTOR_COMMAND::CMD_NOP) << '\n'
                 << "CmdTimepoint = 0\n"
                 << "CmdRepeatable = false\n"
                 << std::endl;
    std::filesystem::resize_file(m_CommandFile, m_FileStream.tellp()); /* 设置文件 EOF */
}
void Messenger::Dispatch(const ExecutorCommand& ec) noexcept
{
    std::lock_guard<std::mutex> lk(m_Mutex);
    static std::uint64_t id_record = 0;
    if (id_record > ec.GetId())
    {
        return;
    }
    m_CommandString = QueryCommandString(ec.GetCmd());
    m_CommandTimepoint = ec.GetCmdTimepoint();
    m_FileStream.seekp(0);
    m_FileStream << "CmdId = " << ec.GetId() << '\n'
                 << "CmdType = " << m_CommandString << '\n'
                 << "CmdTimepoint = " << m_CommandTimepoint << '\n'
                 << "CmdRepeatable = " << (ec.IsRepeatable() ? "true" : "false") << std::endl;
    std::filesystem::resize_file(m_CommandFile, m_FileStream.tellp()); /* 设置文件 EOF */
}

constexpr const char* Messenger::QueryCommandString(EXECUTOR_COMMAND cmd) noexcept
{
    switch (cmd)
    {
    case EXECUTOR_COMMAND::CMD_NOP:
        return "Command.CMD_NOP";
    case EXECUTOR_COMMAND::CMD_START_GAME_ROOM:
        return "Command.CMD_START_GAME_ROOM";
    case EXECUTOR_COMMAND::CMD_CHOOSE_CHARACTER:
        return "Command.CMD_CHOOSE_CHARACTER";
    case EXECUTOR_COMMAND::CMD_DEFAULT_IDLE:
        return "Command.CMD_DEFAULT_IDLE";
    case EXECUTOR_COMMAND::CMD_EXTENDED_IDLE:
        return "Command.CMD_EXTENDED_IDLE";
    case EXECUTOR_COMMAND::CMD_CONFIRM_RESULTS:
        return "Command.CMD_CONFIRM_RESULTS";
    case EXECUTOR_COMMAND::CMD_CREATE_GAME_ROOM:
        return "Command.CMD_CREATE_GAME_ROOM";
    case EXECUTOR_COMMAND::CMD_BATCH_COMBINE_PARTS:
        return "Command.CMD_BATCH_COMBINE_PARTS";
    case EXECUTOR_COMMAND::CMD_BATCH_PURCHASE_ITEM:
        return "Command.CMD_BATCH_PURCHASE_ITEM";
    case EXECUTOR_COMMAND::CMD_LOCATE_CURSOR:
        return "Command.CMD_LOCATE_CURSOR";
    case EXECUTOR_COMMAND::CMD_CLEAR_POPUPS:
        return "Command.CMD_CLEAR_POPUPS";
    default:
        return "Command.NOP";
    }
}
}
