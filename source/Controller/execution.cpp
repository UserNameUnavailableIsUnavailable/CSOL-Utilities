#include <Windows.h>
#include <fileapi.h>
#include <synchapi.h>
#include <winnt.h>

HANDLE g_hStartGameEvent;
HANDLE g_hPlayGameEvent;
HANDLE g_hConfirmRoundEvent;

HANDLE g_hStartGameThread;
HANDLE g_hPlayGameThread;
HANDLE g_hConfirmGameThread;

HANDLE g_hCmdFile;

#define CMD_START_GAME "command = CMD_START_GAME"

void give_command(LPCSTR lpszCmd, DWORD cbLength)
{
    // HANDLE hCmdFile = CreateFileW(
    //     L".\\~command.lua",
    //     GENERIC_READ | GENERIC_WRITE,
    //     FILE_SHARE_READ, /* do not open this file while modifying it */
    //     nullptr,
    //     CREATE_ALWAYS, /* create (not exists) or clear this file (already exists) */
    //     FILE_ATTRIBUTE_HIDDEN, /* hidden file */
    //     nullptr
    // );
    /*
    Writes command to lua script, this will cause a click on START GAME button.
    */
    SetFilePointer(
        g_hCmdFile,
        0,
        0,
        FILE_BEGIN
    );
    WriteFile(
        g_hCmdFile,
        lpszCmd, /* command to start game */
        cbLength,
        nullptr,
        nullptr
    );
    SetEndOfFile(g_hCmdFile);
}

/*
@brief
*/
void initialize_game()
{
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

    /* Read Error.log, get game state */
}
/*
@brief start game by clicking the Start button
*/
void start_game()
{
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