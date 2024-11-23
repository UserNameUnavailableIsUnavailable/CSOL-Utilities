#pragma once

#include "CSOL_Utilities.hpp"
#include <chrono>
#include <ctime>

namespace CSOL_Utilities
{
class InGameState
{
  private:
    CSOL_Utilities::IN_GAME_STATE m_InGameState;
    std::time_t m_Timestamp;
    bool m_LastUpdateOk;

  public:
    InGameState() noexcept
        : m_InGameState(CSOL_Utilities::IN_GAME_STATE::IGS_UNKNOWN), m_Timestamp(0), m_LastUpdateOk(true) {};
    InGameState(CSOL_Utilities::IN_GAME_STATE state, std::time_t timestamp) noexcept
        : m_InGameState(state), m_Timestamp(timestamp), m_LastUpdateOk(true) {};
    InGameState &update(CSOL_Utilities::IN_GAME_STATE in_game_state, std::time_t timestamp, bool bIgnoreTimeForSameState = true) noexcept
    {
        /* 不允许用旧状态覆盖新状态 */
        if (m_Timestamp > timestamp)
        {
            m_LastUpdateOk = false;
            return *this;
        }
        /* 对于完全相同（状态、时间完全相同）的状态不执行更新 */
        if (in_game_state == m_InGameState && timestamp == m_Timestamp)
        {
            m_LastUpdateOk = false;
            return *this;
        }
        /* 不允许更新到当前时间点之后的时间 */
        if (m_Timestamp > std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
        {
            m_LastUpdateOk = false;
            return *this;
        }
        if (m_InGameState != in_game_state || !bIgnoreTimeForSameState)
        {
            m_Timestamp = timestamp;
        }
        m_InGameState = in_game_state;
        m_LastUpdateOk = true;
        return *this;
    }
    /*
    @brief 更新 `GameState` 对象。
    @param `gs` 当前日志对象更新为 `gs`。
    @note 如更新成功，返回 `true`，否则返回 `false`。`gs` 时间戳为
    `0`（无效时间戳）或 `gs` 与 `*this` 完全相同会导致更新失败。
    */
    InGameState &update(const InGameState &gs, bool bIgnoreTimeForSameState = true) noexcept
    {
        if (gs.m_Timestamp == 0)
        {
            m_LastUpdateOk = false;
            return *this; /* 无效 GameState */
        }
        if (gs.m_Timestamp > this->m_Timestamp)
        {
            m_LastUpdateOk = false;
            return *this; /* 尝试用旧状态覆盖新状态 */
        }
        if (gs == *this)
        {
            m_LastUpdateOk = false;
            return *this; /* 相同，无需更新 */
        }
        if (m_InGameState != gs.m_InGameState || !bIgnoreTimeForSameState)
        {
            m_Timestamp = gs.m_Timestamp;
        }
        m_InGameState = gs.m_InGameState;
        m_LastUpdateOk = true;
        return *this;
    }
    inline bool operator==(const InGameState &gs) const noexcept
    {
        return this->m_InGameState == gs.m_InGameState && this->m_Timestamp == gs.m_Timestamp;
    }
    inline bool operator!=(const InGameState &gs) const noexcept
    {
        return this->m_Timestamp != gs.m_Timestamp || this->m_InGameState != gs.m_InGameState;
    }
    InGameState &operator=(const InGameState &gs) noexcept
    {
        return update(gs);
    }
    inline auto GetState() const noexcept
    {
        return m_InGameState;
    }
    inline auto GetTimestamp() const noexcept
    {
        return m_Timestamp;
    }
    inline auto IsLastUpdateSuccessful() const noexcept
    {
        return m_LastUpdateOk;
    }
    inline void reset() noexcept
    {
        m_InGameState = IN_GAME_STATE::IGS_UNKNOWN;
        m_Timestamp = 0;
        m_LastUpdateOk = true;
    }
};
}; // namespace CSOL_Utilities
