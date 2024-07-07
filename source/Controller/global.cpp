#include <Windows.h>
#include "global.hpp"
#include <memory>

HANDLE g_hStartGameEvent; /* the event that triggers the game to start */
HANDLE g_hPlayGameEvent; /* the event that causes the lua script to play game */
HANDLE g_hConfirmRoundEvent; /* the event that causes the lua script to confirm a round */

HANDLE g_hStartGameThread;
HANDLE g_hPlayGameThread;
HANDLE g_hConfirmGameThread;

HANDLE g_hCmdFile;
HANDLE g_hLogFile;

std::shared_ptr<wchar_t []> g_ErrorLogPath;