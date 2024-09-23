#include <Windows.h>
#include <cstdio>
#include "CController.hpp"
#include "CConsole.hpp"
#include "CException.hpp"

using namespace CSOL_Utilities;

bool g_bDestroy{ false };

static BOOL OnDestroyConsole(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT) {
        CController::DestroyInstance();
        return TRUE;
    } else if (dwCtrlType == CTRL_CLOSE_EVENT) {
        CController::DestroyInstance();
        return TRUE;
    } else {
        return FALSE;
    }
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    using namespace CSOL_Utilities;
    if (!CConsole::Configure(OnDestroyConsole)) {
        std::puts("【错误】程序运行时遇到严重错误，无法继续运行，按任意键退出。");
        std::getchar();
        return GetLastError();
    }
    if (!IsRunningAsAdmin()) {
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_WARNING, "程序未以管理员权限运行，这会导致掉线重连失败。");
    }
    try {
        auto game_path = ConvertUtf16ToUtf8(QueryRegistryStringItem(HKEY_CURRENT_USER, L"Software\\TCGame\\csol", L"gamepath").get());
        auto launcher_path = ConvertUtf16ToUtf8(QueryRegistryStringItem(HKEY_CURRENT_USER, L"Software\\TCGame", L"setup").get());
        CController::InitializeInstance(game_path.get(), launcher_path.get());
        CController& instance = CController::RetrieveInstance();
        instance.RunInstance();
        CController::DestroyInstance();
    } catch (CException& e) {
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, e.what());
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, "程序运行时遇到严重错误，无法继续运行，按任意键退出。");
        CController::DestroyInstance();
        std::getchar();
        return GetLastError();
    }
    return ERROR_SUCCESS;
}