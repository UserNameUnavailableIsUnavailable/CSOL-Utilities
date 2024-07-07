#include <Windows.h>
#include <cwchar>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <memory>
#include <synchapi.h>
#include <winnt.h>
#include "csol24except.hpp"
#include "global.hpp"
#include "command.hpp"
#include "game.hpp"
#include <regex>
#include <winscard.h>

#define CMD_START_GAME "command = CMD_START_GAME"

enum GameState
{
    GS_CONNECT_TO_HOST,
    GS_CONFIRM_RESULT,
    GS_CREATE_ROOM,
    GS_LEAVE_ROOM
};

void check_game_state()
{

}


/*
@brief
*/
void initialize_game()
{
    g_hCmdFile = CreateFileW(
        L".\\command.lua",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS, /* create (not exists) or clear this file (already exists) */
        FILE_ATTRIBUTE_HIDDEN, /* hidden file */
        nullptr
    );
    g_hStartGameEvent = CreateEventW(nullptr, true, false, L"g_hStartGameEvent");
    g_hPlayGameEvent = CreateEventW(nullptr, true, false, L"g_hPlayGameEvent");
    g_hConfirmRoundEvent = CreateEventW(nullptr, true, false, L"g_hConfirmRoundEvent");
    // TODO: error handling
    g_hStartGameThread = CreateThread(
        nullptr,
        4096,
        nullptr, // TODO
        nullptr,
        0, /* create running */
        nullptr
    );
    g_hPlayGameThread = CreateThread(
        nullptr,
        4096,
        nullptr, // TODO
        nullptr,
        0, /* create running */
        nullptr
    );
    g_hConfirmGameThread = CreateThread(
        nullptr,
        4096,
        nullptr, // TODO
        nullptr,
        0, /* create running */
        nullptr
    );
    // TODO: error handling

    /* Get the location of Error.log */
    try
    {
        std::shared_ptr<wchar_t[]> gameInstallationPath = query_installation_path(); /* Query CSOL installation path */
        size_t error_log_path_length = 1 + wcslen(gameInstallationPath.get()) + wcslen(L"\\bin\\Error.log"); /* length of error.log path */
        g_ErrorLogPath = std::shared_ptr<wchar_t[]>(new wchar_t[error_log_path_length]);
        wcscpy_s(g_ErrorLogPath.get(), error_log_path_length, gameInstallationPath.get());
        wcscat_s(g_ErrorLogPath.get(), error_log_path_length, L"\\bin\\Error.log");
    }
    catch (CSOL24EXCEPT e)
    {
        NotifyError(e.what());
        ExitProcess(-1);
    }
    g_hLogFile = CreateFileW(
        g_ErrorLogPath.get(), 
        GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, 
        nullptr, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        nullptr
    );
    if (g_hLogFile == INVALID_HANDLE_VALUE)
    {
        NotifyError("打开文件失败，错误代码：%lu。文件名：%ls。", GetLastError(), g_ErrorLogPath.get());
    }
}
/*
@brief start game by clicking the Start button
*/
void start_game()
{
    give_command(CMD_START_GAME, sizeof(CMD_START_GAME) / sizeof(char));
    /* Basically, it takes less than 20 s to load */
    Sleep(30 * 1000); /* Wait 30 s for the round to start */
}

/*
@brief give CMD_PLAY_GAME command to lua script
*/
void play_game()
{

}


/*
@brief pause the execution of the script
*/
void pause_game()
{

}