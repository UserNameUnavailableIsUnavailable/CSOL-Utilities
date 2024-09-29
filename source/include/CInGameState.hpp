#pragma once

#include "CSOL_Utilities.hpp"
#include <chrono>
#include <ctime>

namespace CSOL_Utilities
{
class CInGameState
{
  private:
    CSOL_Utilities::IN_GAME_STATE m_InGameState;
    std::time_t m_Timestamp;
    bool m_LastUpdateOk;

  public:
    CInGameState() noexcept
        : m_InGameState(CSOL_Utilities::IN_GAME_STATE::IGS_UNKNOWN), m_Timestamp(0), m_LastUpdateOk(true) {};
    CInGameState(CSOL_Utilities::IN_GAME_STATE state, std::time_t timestamp) noexcept
        : m_InGameState(state), m_Timestamp(timestamp), m_LastUpdateOk(true) {};
    CInGameState &update(CSOL_Utilities::IN_GAME_STATE in_game_state, std::time_t timestamp)
    {
        /* 不允许用旧状态覆盖新状态 */
        if (this->m_Timestamp > timestamp)
        {
            m_LastUpdateOk = false;
            return *this;
        }
        /* 对于完全相同的状态不执行更新 */
        if (in_game_state == this->m_InGameState && timestamp == this->m_Timestamp)
        {
            m_LastUpdateOk = false;
            return *this;
        }
        /* 不允许更新到当前时间点之后的时间 */
        if (this->m_Timestamp > std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
        {
            m_LastUpdateOk = false;
            return *this;
        }
        this->m_InGameState = in_game_state;
        this->m_Timestamp = timestamp;
        m_LastUpdateOk = true;
        return *this;
    }
    /*
    @brief 更新 `GameState` 对象。
    @param `gs` 当前日志对象更新为 `gs`。
    @note 如更新成功，返回 `true`，否则返回 `false`。`gs` 时间戳为
    `0`（无效时间戳）或 `gs` 与 `*this` 完全相同会导致更新失败。
    */
    CInGameState &update(const CInGameState &gs) noexcept
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
        this->m_InGameState = gs.m_InGameState;
        this->m_Timestamp = gs.m_Timestamp;
        m_LastUpdateOk = true;
        return *this;
    }
    inline bool operator==(const CInGameState &gs) const noexcept
    {
        return this->m_InGameState == gs.m_InGameState && this->m_Timestamp == gs.m_Timestamp;
    }
    inline bool operator!=(const CInGameState &gs) const noexcept
    {
        return this->m_Timestamp != gs.m_Timestamp || this->m_InGameState != gs.m_InGameState;
    }
    CInGameState &operator=(const CInGameState &gs) noexcept
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
