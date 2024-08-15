#include "CSOL24H.hpp"
#include "Command.hpp"
#include <Windows.h>
#include <synchapi.h>
#include "Console.hpp"

DWORD CSOL24H::CombineParts(LPVOID lpParam) noexcept
{
    while (WAIT_OBJECT_0 == WaitForSingleObject(hEnableCombinePartsEvent, INFINITE))
    {
        if (bDestroy)
        {
            break;
        }
        GiveCommand(LUA_CMD_COMBINE_PARTS);
        Sleep(1000);
    }
    ConsoleLog("线程 hCombinePartsThread 退出。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE);
    return 0;
}