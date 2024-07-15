#include "CSOL24H.hpp"
#include "CSOL24H_EXCEPT.hpp"
#include <cstddef>
#include <cstdio>
#include <cwchar>
#include <iostream>
#include <synchapi.h>
#include <wincon.h>
#include "Console.hpp"

int main()
{
    if (!InitializeConsole())
    {
        std::printf("【错误】初始化控制台失败。请按任意键退出。");
        std::getchar();
        return -1;
    }
    try
    {
        CSOL24H::Initialize();
        CSOL24H::Run();
    }
    catch (CSOL24H_EXCEPT e)
    {
        std::cout << e.what() << std::endl;
        ConsoleLog("%s\r\n", e.what());
        CSOL24H::Destroy();
        ConsoleLog("【错误】程序无法继续运行。按任意键退出。");
        std::getchar();
    }
    /* finally */
    DestroyConsole();
    return 0;
}