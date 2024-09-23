#include "CController.hpp"
#include "CConsole.hpp"
#include "CMessenger.hpp"
#include <Windows.h>
#include <chrono>
#include <thread>

using namespace CSOL_Utilities;

void CController::DispatchFixedCommand(COMMAND& command) noexcept
{
    while (!s_Instance->m_bExitThreads) {
        s_Instance->m_FixedCommandDispatcherSwitch.Wait();
        s_Instance->m_FixedCommandDispatcherFinished.Reset();
        s_Instance->m_Messenger.assign(command, std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()).dispatch();
        s_Instance->m_FixedCommandDispatcherFinished.Set();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 m_DispatchFixedCommand 退出。");
    s_Instance->m_bThreadExit.Set();
}