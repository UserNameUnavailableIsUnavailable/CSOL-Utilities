#pragma once

namespace CSOL_Utilities
{
    using IGS_T = enum class IN_GAME_STATE_TYPE
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
}