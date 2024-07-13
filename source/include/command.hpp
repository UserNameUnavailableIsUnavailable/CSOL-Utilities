#pragma once
#include <Windows.h>
#include <string>
#include <cwchar>

static const wchar_t* CMD_FILE_NAME = L"C:\\Users\\Silve\\Develop\\CSOL-24H\\source\\lua\\$~cmd.lua";
static const char* LUA_CMD_NOP = u8"Command.CMD_NOP";
static const char* LUA_CMD_START_GAME_ROOM  = u8"Command.CMD_START_GAME_ROOM";
static const char* LUA_CMD_CHOOSE_CLASS = u8"Command.CMD_CHOOSE_CLASS";
static const char* LUA_CMD_PLAY_GAME = u8"Command.CMD_PLAY_GAME";
static const char* LUA_CMD_PLAY_GAME_EXTEND = u8"Command.CMD_PLAY_GAME_EXTEND";
static const char* LUA_CMD_TRY_CONFIRM_RESULT = u8"Command.CMD_TRY_CONFIRM_RESULT";
static const char* LUA_CMD_CREATE_ROOM = u8"Command.CMD_CREATE_ROOM";
static const char* LUA_CMD_CRAFT_COMBINE_PARTS = u8"Command.CMD_CRAFT_COMBINE_PARTS";
static const char* LUA_CMD_STORE_PURCHASE_ITEM = u8"Command.CMD_STORE_PURCHASE_ITEM";
static const char* LUA_COMMAND_FORMAT = u8"Cmd = %s\n" u8"CmdTime = %llu\n";