#include "CConsole.hpp"
#include "CController.hpp"
#include "CLI11.hpp"
#include <Windows.h>
#include <cassert>
#include <cstdio>
#include <exception>
#include <memory>
#include <string>
#include <wincon.h>

using namespace CSOL_Utilities;

bool g_bDestroy{false};

static BOOL OnDestroyConsole(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT)
    {
        CController::DestroyInstance();
        return TRUE;
    }
    else if (dwCtrlType == CTRL_CLOSE_EVENT)
    {
        CController::DestroyInstance();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

int wmain(int argc, wchar_t **argv)
{
    using namespace CSOL_Utilities;
    if (!CConsole::Configure(OnDestroyConsole))
    {
        std::puts("【错误】程序运行时遇到严重错误，无法继续运行，按任意键退出。");
        std::getchar();
        return GetLastError();
    }
    if (!IsRunningAsAdmin())
    {
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_WARNING, "程序未以管理员权限运行，这会导致掉线重连失败。");
    }
    CLI::App app{"CSOL 集成工具"};
    std::string game_root_dir;
    std::string launch_game_cmd;
    uint32_t max_wait_time_in_room{15 * 60};
    app.add_option<std::string>("--GameRootDirectory", game_root_dir, "Game root path, e.g., \"C:\\csol\"");
    app.add_option<std::string>(
        "--LaunchGameCmd", launch_game_cmd,
        "Command line to launch the game, e.g., \"C:\\TCGAME\\tcgame.exe\" cso. Note that the program path must be "
        "parenthesized, otherwise the controller will fail to aoto-restart the game.");
    app.add_option<uint32_t>(
        "--MaxWaitTimeInRoom", max_wait_time_in_room,
        "Maximum time to wait in a room, in seconds, 900 seconds by default. Once the in-room waiting time exceeds "
        "this value, controller will instruct the executor to leave the game room and create a new room.");
    CLI11_PARSE(app, argc, argv);
    try
    {
        if (launch_game_cmd.length() == 0)
        {
            launch_game_cmd =
                ConvertUtf16ToUtf8(QueryRegistryStringItem(HKEY_CURRENT_USER, L"Software\\TCGame", L"setup").get())
                    .get();
            launch_game_cmd = '\"' + launch_game_cmd + "\\TCGame.exe" + '\"' + " " + "cso";
        }
        if (game_root_dir.length() == 0)
        {
            game_root_dir =
                ConvertUtf16ToUtf8(
                    QueryRegistryStringItem(HKEY_CURRENT_USER, L"Software\\TCGame\\csol", L"gamepath").get())
                    .get();
        }
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE,
                      "本工具由 _CoreDump 开发。联系邮箱：ttyuig@126.com，B 站 "
                      "ID：_CoreDump。本工具开源免费，请注意甄别。项目地址：https://gitee.com/silver1867/csol-24-h。");
        CController::InitializeInstance(std::move(game_root_dir), std::move(launch_game_cmd));
        CController &instance = CController::RetrieveInstance();
        instance.SetMaxWaitTimeInGameRoom(max_wait_time_in_room);
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "在房间内等待最长时间设定为 %u 秒。",
                      instance.GetMaxWaitTimeInGameRoom());
        instance.RunInstance();
        CController::DestroyInstance();
    }
    catch (std::exception &e)
    {
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, e.what());
        CController::DestroyInstance();
        CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, "程序运行时遇到严重错误，无法继续运行，请按任意键退出程序。");
        std::getchar();
        return GetLastError();
    }
    return ERROR_SUCCESS;
}