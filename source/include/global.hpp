#pragma once
#include <Windows.h>
#include <memory>

extern HANDLE g_hStartGameEvent; /* the event that triggers the game to start */
extern HANDLE g_hPlayGameEvent; /* the event that causes the lua script to play game */
extern HANDLE g_hConfirmRoundEvent; /* the event that causes the lua script to confirm a round */
extern HANDLE g_hStartGameThread;
extern HANDLE g_hPlayGameThread;
extern HANDLE g_hConfirmGameThread;
extern HANDLE g_hCmdFile;
extern HANDLE g_hLogFile;
extern std::shared_ptr<wchar_t[]> g_ErrorLogPath;
