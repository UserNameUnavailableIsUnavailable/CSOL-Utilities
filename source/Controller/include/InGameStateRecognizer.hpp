#pragma once
#include "CSOL_Utilities.hpp"
#include <string>
#include <thread>

namespace CSOL_Utilities
{
// 识别游戏内状态
class InGameStateRecognizer
{
public:
    IN_GAME_STATE Detect() noexcept;
    IN_GAME_STATE GetState() noexcept
    {
        return m_GameState;
    }

private:
    IN_GAME_STATE m_GameState = IN_GAME_STATE::IGS_UNKNOWN; // 游戏状态
    std::wstring m_LGHUBAgentProcessName;
    DWORD m_LGHUBAgentProcessId = 0; // LGHUB Agent 进程号
    HANDLE m_LGHUBAgentProcessHandle = NULL; // LGHUB Agent 进程句柄
    std::thread m_LGHUBAgentDebugThread;
    void DebugAgent() noexcept;
};
}
