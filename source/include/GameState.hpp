#pragma once

#include <cstdint>
#include <chrono>

/* 游戏状态 */
enum ENUM_GAME_STATE
{
    GS_LOGIN,
    GS_HALL,
    GS_LOADING,
    GS_ROOM,
    GS_MAP,
    GS_SHUTDOWN,
    GS_UNKNOWN
};

class GameState
{
private:
    ENUM_GAME_STATE game_state;
    int64_t timestamp;
public:
    GameState() : game_state(ENUM_GAME_STATE::GS_UNKNOWN), timestamp(0) {};
    GameState(ENUM_GAME_STATE state, int64_t ts) : game_state(state), timestamp(ts) {};
    ~GameState() = default;
    inline void update(ENUM_GAME_STATE game_state, int64_t timestamp) noexcept
    {
        this->game_state = game_state;
        this->timestamp = timestamp;
    }
    /*
    @brief 更新 `GameState` 对象。
    @param `gs` 当前日志对象更新为 `gs`。
    @note 如更新成功，返回 `true`，否则返回 `false`。`gs` 时间戳为 `0`（无效时间戳）或 `gs` 与 `*this` 完全相同会导致更新失败。
    */
    bool update(const GameState& gs) noexcept
    {
        if (gs.timestamp == 0) return false; /* 无效 GameState */
        if (gs == *this) return false; /* 两个 GameState 相同，无需更新 */
        this->game_state = gs.game_state;
        this->timestamp = gs.timestamp;
        return true;
    }
    inline bool operator==(const GameState& gs) const
    {
        return this->game_state == gs.game_state && this->timestamp == gs.timestamp;
    }
    inline bool operator!=(const GameState& gs) const
    {
        return this->timestamp != gs.timestamp || this->game_state != gs.game_state;
    }
    GameState& operator=(const GameState& gs)
    {
        if (gs.timestamp == 0) return *this;
        if (gs == *this) return *this; /* 两个 GameState 相同，无需更新 */
        this->game_state = gs.game_state;
        this->timestamp = gs.timestamp;
        return *this;
    }
    inline ENUM_GAME_STATE get_state() { return game_state; }
    inline int64_t get_timestamp() { return timestamp; }
};