namespace CSOL_Utilities
{
    using GPS_T = enum class GAME_PROCESS_STATE_TYPE
    {
        GPS_BEING_CREATED, /* 游戏进程正在被创建 */
        GPS_RUNNING,       /* 游戏进程正在运行 */
        GPS_EXITED,        /* 游戏进程退出 */
        GPS_UNKNOWN,       /* 尚未确认游戏进程状态 */
    };
}