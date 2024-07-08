#pragma once
#include <Windows.h>

#define CMD_START_GAME "command = CMD_START_GAME"

void give_command(LPCSTR lpszCmd, DWORD cbLength);
