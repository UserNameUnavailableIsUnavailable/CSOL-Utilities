#include <Windows.h>
#include "global.hpp"
#include "command.hpp"

void give_command(LPCSTR lpszCmd, DWORD cbLength)
{
    /*
    Writes command to lua script, this will cause a click on START GAME button.
    */
    SetFilePointer(
        g_hCmdFile,
        0,
        0,
        FILE_BEGIN
    );
    WriteFile(
        g_hCmdFile,
        lpszCmd, /* command to start game */
        cbLength,
        nullptr,
        nullptr
    );
    SetEndOfFile(g_hCmdFile);
}
