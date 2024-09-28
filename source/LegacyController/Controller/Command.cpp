#include <Windows.h>
#include <CSOL24H.hpp>
#include "Command.hpp"
#include <ctime>
#include <fileapi.h>
#include <synchapi.h>
#include <winerror.h>

void CSOL24H::GiveCommand(const char* cmd) noexcept
{
    DWORD dwRet = WaitForSingleObject(hCmdFileMutex, 10);
    if (dwRet != WAIT_OBJECT_0) return; /* 等待互斥量超时或因为其他原因失败 */
    SetFilePointer(
        hLUACommandFile,
        0,
        0,
        FILE_BEGIN
    );
    if (!cmd)
    {
        SetEndOfFile(hLUACommandFile);
        return;
    }
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
    ReleaseMutex(hCmdFileMutex);
}
