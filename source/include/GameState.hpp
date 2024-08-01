#pragma once

#include <cstdint>
#include <chrono>

/* 游戏状态 */
enum ENUM_IN_GAME_STATE
{
    IGS_LOGIN, /* 正在登陆 */
    IGS_IN_HALL, /* 在大厅中 */
    IGS_IN_ROOM, /* 在房间内，尚未开始游戏 */
    IGS_IN_ROOM_ABNORMAL, /* 在房间内（因网络波动等异常原因导致的回到房间） */
    IGS_LOADING, /* 游戏场景正在加载 */
    IGS_IN_MAP, /* 在游戏地图中 */
    IGS_SHUTDOWN, /* 游戏关闭 */
    IGS_UNKNOWN /* 未知状态 */
};

enum ENUM_GAME_PROCESS_STATE
{
    GPS_BEING_CREATED, /* 游戏进程正在被创建 */
    GPS_RUNNING, /* 游戏进程正在运行 */
    GPS_EXITED, /* 游戏进程退出 */
    GPS_UNKNOWN, /* 尚未确认游戏进程状态 */
};

class InGameState
{
private:
    ENUM_IN_GAME_STATE in_game_state;
    int64_t timestamp;
public:
    InGameState() : in_game_state(ENUM_IN_GAME_STATE::IGS_UNKNOWN), timestamp(0) {};
    InGameState(ENUM_IN_GAME_STATE state, int64_t ts) : in_game_state(state), timestamp(ts) {};
    ~InGameState() = default;
    inline void update(ENUM_IN_GAME_STATE in_game_state, int64_t timestamp) noexcept
    {
        this->in_game_state = in_game_state;
        this->timestamp = timestamp;
    }
    /*
    @brief 更新 `GameState` 对象。
    @param `gs` 当前日志对象更新为 `gs`。
    @note 如更新成功，返回 `true`，否则返回 `false`。`gs` 时间戳为 `0`（无效时间戳）或 `gs` 与 `*this` 完全相同会导致更新失败。
    */
    bool update(const InGameState& gs) noexcept
    {
        if (gs.timestamp == 0) return false; /* 无效 GameState */
        if (gs == *this) return false; /* 两个 GameState 相同，无需更新 */
        this->in_game_state = gs.in_game_state;
        this->timestamp = gs.timestamp;
        return true;
    }
    inline bool operator==(const InGameState& gs) const
    {
        return this->in_game_state == gs.in_game_state && this->timestamp == gs.timestamp;
    }
    inline bool operator!=(const InGameState& gs) const
    {
        return this->timestamp != gs.timestamp || this->in_game_state != gs.in_game_state;
    }
    InGameState& operator=(const InGameState& gs)
    {
        if (gs.timestamp == 0) return *this;
        if (gs == *this) return *this; /* 两个 GameState 相同，无需更新 */
        this->in_game_state = gs.in_game_state;
        this->timestamp = gs.timestamp;
        return *this;
    }
    inline ENUM_IN_GAME_STATE get_state() { return in_game_state; }
    inline int64_t get_timestamp() { return timestamp; }
};