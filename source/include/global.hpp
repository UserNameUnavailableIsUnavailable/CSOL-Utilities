#pragma once
#include <BaseTsd.h>
#include <Windows.h>
#include <memory>
#include "game.hpp"

extern HANDLE g_hStartGameEvent; /* the event that triggers the game to start */
extern HANDLE g_hPlayGameEvent; /* the event that causes the lua script to play game */
extern HANDLE g_hConfirmRoundEvent; /* the event that causes the lua script to confirm a round */
extern HANDLE g_hStartGameWatcherEvent; /* the event that causes the game watcher thread to start */

extern HANDLE g_hGameStateWatcherThread; /* watch game state real-time */
extern HANDLE g_hStartGameThread;
extern HANDLE g_hPlayGameThread;
extern HANDLE g_hConfirmGameThread;

extern HANDLE g_hCmdFile;
extern HANDLE g_hLogFile;
extern UINT64 g_qwLogFileSize;
extern UINT64 g_qwLogFileLastModifyTime;

extern BOOL g_bExit; /* will cause the process to exit */

extern ENUM_GAME_STATE g_GameState;
extern std::shared_ptr<wchar_t[]> g_ErrorLogPath;
