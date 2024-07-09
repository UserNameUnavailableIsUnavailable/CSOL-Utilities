#pragma once
#include <Windows.h>
#include <string>

const std::string CMD_START_GAME_ROOM = "command = CMD_START_GAME_ROOM";
const std::string CMD_TRY_CONFIRM_RESULT = "command = CMD_TRY_CONFIRM_RESULT";
const std::string CMD_CREATE_ROOM = "command = CMD_CREATE_ROOM";

namespace CSOL24H
{
    void GiveCommand(std::string cmd);
};

