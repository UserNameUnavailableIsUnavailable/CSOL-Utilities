#include <Windows.h>
#include <CSOL24H.hpp>
#include "command.hpp"
#include <ctime>

void CSOL24H::GiveCommand(const char* cmd) noexcept
{
    /*
    Writes command to lua script, this will cause a click on START GAME button.
    */
    SetFilePointer(
        hCmdFile,
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
        hCmdFile,
        buffer, /* command to start game */
        length,
        nullptr,
        nullptr
    );
    SetEndOfFile(hCmdFile);
}
