#include <Windows.h>
#include <cwchar>
#include <fileapi.h>
#include <memory>
#include <synchapi.h>
#include <winnt.h>
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
    /* Get the location of Error.log */
    std::shared_ptr<wchar_t[]> gameInstallationPath = query_installation_path();
    auto qwLength = std::wcslen(gameInstallationPath.get()) + std::wcslen(L"\\bin\\Error.log");
    g_ErrorLogPath = std::shared_ptr<wchar_t[]>(new wchar_t[qwLength]);
    g_hLogFile = CreateFileW(
        g_ErrorLogPath.get(), 
        GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, 
        nullptr, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        nullptr
    );
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