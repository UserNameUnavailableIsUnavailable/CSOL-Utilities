#pragma once
#include <Windows.h>
#include <memory>
#include "util.hpp"

enum ENUM_CSOL_GAME_COMMAND
{
    CSOL_NOP, // 此时无需任何操作
    CSOL_QUERY_CURSOR_LOCATION, // 获取鼠标光标位置，需要打开罗技控制台查看
    CSOL_ROUND_WAIT, // 在房间等待
    CSOL_ROUND_LOAD, // 游戏正在加载
    CSOL_ROUND_CHOOSE_CLASS, // 选择
    CSOL_ROUND_PLAY, // 游戏正在进行
    CSOL_ROUND_CONFIRM, // 游戏结算（若成功结算，则转入 CSOL_ROUND_WAIT）
    CSOL_ROUND_TIMEOUT, // 超时机制
    CSOL_CRAFT_PARTS_COMBINE, // 制造所合成配件
    CSOL_STORE_PURCHASE, // 商店购买物品
};

std::shared_ptr<wchar_t[]> query_installation_path(
    REG_PREDEFINED_KEY_ENUM predefinedTopDir = REG_PREDEFINED_KEY_ENUM::REG_CURRENT_USER,
    LPCWSTR lpSubDir = L"Software\\TCGame\\csol",
    LPCWSTR lpItemName = L"gamepath"
);
