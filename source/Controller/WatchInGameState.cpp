#include "CCommander.hpp"

using namespace CSOL_Utilities;

void CCommander::WatchInGameState() noexcept
{
    s_Obj->m_InGameStateWatcherSwitch.lock();   
}