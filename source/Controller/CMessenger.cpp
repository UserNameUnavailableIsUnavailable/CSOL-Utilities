#include "CMessenger.hpp"
#include <filesystem>
#include <mutex>

CSOL_Utilities::CMessenger::CMessenger(std::filesystem::path file)
    : m_CommandFile(file), m_FileStream(m_CommandFile, std::ios::out | std::ios::binary),
      m_CommandString(QueryCommandString(COMMAND::CMD_NOP)), m_Mutex(), m_Buffer(new char[512])
{
}

CSOL_Utilities::CMessenger::~CMessenger() noexcept
{
    AssignAndDispatch(CSOL_Utilities::COMMAND::CMD_NOP, 0);
    delete[] m_Buffer;
}

void CSOL_Utilities::CMessenger::AssignAndDispatch(const CSOL_Utilities::COMMAND cmd, std::time_t cmd_time) noexcept
{
    std::lock_guard<std::mutex> lock_guard(m_Mutex);
    m_CommandString = QueryCommandString(cmd);
    m_CommandTime = cmd_time;
    m_FileStream.seekp(0);
    m_FileStream << "Cmd = " << m_CommandString << '\n' /* 使用 \n 而非 std::endl，目的是暂时不清空 buffer */
                 << "CmdTime = " << m_CommandTime << std::endl;        /* 使用 std::endl 清空 buffer */
    std::filesystem::resize_file(m_CommandFile, m_FileStream.tellp()); /* 设置文件 EOF */
}

constexpr const char *CSOL_Utilities::CMessenger::QueryCommandString(CSOL_Utilities::COMMAND cmd) noexcept
{
    switch (cmd)
    {
    case CSOL_Utilities::COMMAND::CMD_NOP:
        return u8"Command.CMD_NOP";
    case CSOL_Utilities::COMMAND::CMD_START_GAME_ROOM:
        return u8"Command.CMD_START_GAME_ROOM";
    case CSOL_Utilities::COMMAND::CMD_CHOOSE_CLASS:
        return u8"Command.CMD_CHOOSE_CLASS";
    case CSOL_Utilities::COMMAND::CMD_PLAY_GAME_NORMAL:
        return u8"Command.CMD_PLAY_GAME_NORMAL";
    case CSOL_Utilities::COMMAND::CMD_PLAY_GAME_EXTEND:
        return u8"Command.CMD_PLAY_GAME_EXTEND";
    case CSOL_Utilities::COMMAND::CMD_TRY_CONFIRM_RESULT:
        return u8"Command.CMD_TRY_CONFIRM_RESULT";
    case CSOL_Utilities::COMMAND::CMD_CREATE_GAME_ROOM:
        return u8"Command.CMD_CREATE_ROOM";
    case CSOL_Utilities::COMMAND::CMD_COMBINE_PARTS:
        return u8"Command.CMD_COMBINE_PARTS";
    case CSOL_Utilities::COMMAND::CMD_PURCHASE_ITEM:
        return u8"Command.CMD_PURCHASE_ITEM";
    case CSOL_Utilities::COMMAND::CMD_LOCATE_CURSOR:
        return u8"Command.CMD_LOCATE_CURSOR";
    default:
        return u8"Command.NOP";
    }
}