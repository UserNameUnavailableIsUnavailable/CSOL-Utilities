#include <Windows.h>
#include <CSOL24H.hpp>
#include "Command.hpp"
#include <ctime>

void CSOL24H::GiveCommand(const char* cmd) noexcept
{
    SetFilePointer(
        hLUACommandFile,
        0,
        0,
        FILE_BEGIN
    );
    time_t t;
    time(&t); /* timestamp */
    char buffer[512];
    auto length = sprintf_s(
        buffer,
        LUA_COMMAND_FORMAT,
        cmd,
        static_cast<unsigned long long>(t)
    );
    WriteFile(
        hLUACommandFile,
        buffer, /* command to start game */
        length,
        nullptr,
        nullptr
    );
    SetEndOfFile(hLUACommandFile);
}
