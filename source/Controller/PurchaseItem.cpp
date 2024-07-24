#include "CSOL24H.hpp"
#include "Command.hpp"
#include <Windows.h>
#include <minwindef.h>
#include <synchapi.h>
#include <Console.hpp>

DWORD CSOL24H::PurchaseItem(LPVOID lpParam) noexcept
{
    while (WAIT_OBJECT_0 == WaitForSingleObject(hEnablePurchaseItemEvent, INFINITE))
    {
        if (bDestroy)
        {
            break;
        }
        GiveCommand(LUA_CMD_PURCHASE_ITEM);
        Sleep(1000);
    }
    ConsoleLog("【消息】线程 hPurchaseItemThread 退出。\r\n");
    return 0;
}