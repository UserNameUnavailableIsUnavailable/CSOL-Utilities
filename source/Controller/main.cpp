#include "CSOL24H.hpp"
#include "CSOL24H_EXCEPT.hpp"
#include <cstddef>
#include <cstdio>
#include <cwchar>
#include <iostream>
#include <securitybaseapi.h>
#include <synchapi.h>
#include <wincon.h>
#include <winnt.h>
#include "Console.hpp"

BOOL IsRunningAsAdmin() noexcept;

int main()
{
    if (!InitializeConsole())
    {
        std::printf("[Error] Failed to initialize console. Press any key to exit.\r\n");
        std::getchar();
        return -1;
    }
    if (!IsRunningAsAdmin())
    {
        ConsoleLog("控制器未以管理员权限启动，这会导致断线重连失败。", ENUM_CONSOLE_LOG_LEVEL::CLL_WARNING);
    }
    try
    {
        CSOL24H::Initialize();
        CSOL24H::Run();
    }
    catch (CSOL24H_EXCEPT e)
    {
        std::cout << e.what() << std::endl;
        ConsoleLog("%s",ENUM_CONSOLE_LOG_LEVEL::CLL_ERROR, e.what());
        CSOL24H::Destroy();
        ConsoleLog("程序无法继续运行。按任意键退出。", ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE);
        std::getchar();
    }
    /* finally */
    DestroyConsole();
    return 0;
}

BOOL IsRunningAsAdmin() noexcept
{
    BOOL bRet = FALSE;
    PSID admin_group = NULL;

    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &admin_group))
    {
        CheckTokenMembership(NULL, admin_group, &bRet);
        FreeSid(admin_group);
    }
    return bRet;
}