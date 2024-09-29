#pragma once

#include <Windows.h>
#include <memory>

namespace CSOL_Utilities
{
enum COMMAND
{
    CMD_NOP,
    CMD_START_GAME_ROOM,
    CMD_CHOOSE_CLASS,
    CMD_PLAY_GAME_NORMAL,
    CMD_PLAY_GAME_EXTEND,
    CMD_TRY_CONFIRM_RESULT,
    CMD_CREATE_GAME_ROOM,
    CMD_COMBINE_PARTS,
    CMD_PURCHASE_ITEM,
    CMD_LOCATE_CURSOR,
};
/* 游戏状态 */
enum IN_GAME_STATE
{
    IGS_LOGIN,            /* 正在登陆 */
    IGS_IN_HALL,          /* 在大厅中 */
    IGS_IN_ROOM_NORMAL,   /* 在房间内，尚未开始游戏 */
    IGS_IN_ROOM_ABNORMAL, /* 在房间内（因网络波动等异常原因导致的回到房间） */
    IGS_LOADING,          /* 游戏场景正在加载 */
    IGS_IN_MAP,           /* 在游戏地图中 */
    IGS_SHUTDOWN,         /* 游戏关闭 */
    IGS_UNKNOWN           /* 未知状态 */
};
enum GAME_PROCESS_STATE
{
    GPS_BEING_CREATED, /* 游戏进程正在被创建 */
    GPS_RUNNING,       /* 游戏进程正在运行 */
    GPS_EXITED,        /* 游戏进程退出 */
    GPS_UNKNOWN,       /* 尚未确认游戏进程状态 */
};
enum CONSOLE_LOG_LEVEL
{
    CLL_MESSAGE,
    CLL_WARNING,
    CLL_ERROR
};
std::shared_ptr<wchar_t[]> ConvertUtf8ToUtf16(const char *string);
std::shared_ptr<char[]> ConvertUtf16ToUtf8(const wchar_t *string);
std::shared_ptr<wchar_t[]> QueryRegistryStringItem(HKEY hPredefinedTopDir, LPCWSTR lpSubDir, LPCWSTR lpItemName);
BOOL IsRunningAsAdmin() noexcept;
} // namespace CSOL_Utilities