#include <Windows.h>
#include <minwindef.h>
#include <wincon.h>
#include <winreg.h>
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
    if (!CConsole::ConfigureConsole(OnDestroyConsole)) {
        std::puts("【警告】初始化控制台时发生错误。");
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
        CController::DestroyInstance();
    }
}