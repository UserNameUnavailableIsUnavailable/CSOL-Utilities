#include "CMessenger.hpp"
#include <filesystem>

CSOL_Utilities::CMessenger::CMessenger(std::filesystem::path file) :
m_CommandFile(file),
m_FileStream(m_CommandFile, std::ios::out),
m_CommandString(QueryCommandString(COMMAND::NOP)),
m_Mutex(),
m_Buffer(new char[512])
{ }

CSOL_Utilities::CMessenger::~CMessenger() noexcept
{
    assign(CSOL_Utilities::COMMAND::NOP).dispatch();
    delete m_Buffer;
}

inline CSOL_Utilities::CMessenger& CSOL_Utilities::CMessenger::assign(CSOL_Utilities::COMMAND cmd) noexcept
{
    m_CommandString = QueryCommandString(cmd);
    return *this;
}

inline void CSOL_Utilities::CMessenger::dispatch() noexcept
{
    m_Mutex.lock();
    m_FileStream.seekp(0);
    m_FileStream << "Cmd = " << m_CommandString << std::endl
        << "CmdTime = "
        << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
    m_Mutex.unlock();
}

constexpr const char* CSOL_Utilities::CMessenger::QueryCommandString(CSOL_Utilities::COMMAND cmd) noexcept
{
    switch (cmd)
    {
    case CSOL_Utilities::COMMAND::NOP: return u8"Command.CMD_NOP";
    case CSOL_Utilities::COMMAND::START_GAME_ROOM: return u8"Command.CMD_START_GAME_ROOM";
    case CSOL_Utilities::COMMAND::CHOOSE_CLASS: return u8"Command.CMD_CHOOSE_CLASS";
    case CSOL_Utilities::COMMAND::PLAY_GAME_NORMAL: return u8"Command.CMD_PLAY_GAME_NORMAL";
    case CSOL_Utilities::COMMAND::PLAY_GAME_EXTEND: return u8"Command.CMD_PLAY_GAME_EXTEND";
    case CSOL_Utilities::COMMAND::TRY_CONFIRM_RESULT: return u8"Command.CMD_TRY_CONFIRM_RESULT";
    case CSOL_Utilities::COMMAND::CREATE_ROOM: return u8"Command.CMD_CREATE_ROOM";
    case CSOL_Utilities::COMMAND::COMBINE_PARTS: return u8"Command.CMD_COMBINE_PARTS";
    case CSOL_Utilities::COMMAND::PURCHASE_ITEM: return u8"Command.CMD_PURCHASE_ITEM";
    case CSOL_Utilities::COMMAND::LOCATE_CURSOR: return u8"Command.CMD_LOCATE_CURSOR";
    default: return u8"Command.NOP";
    }
}