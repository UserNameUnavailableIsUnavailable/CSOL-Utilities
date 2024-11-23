#include "Console.hpp"
#include "Controller.hpp"
#include "Messenger.hpp"
#include <Windows.h>
#include <chrono>
#include <cstddef>
#include <thread>

using namespace CSOL_Utilities;

void Controller::DispatchFixedCommand(EXECUTOR_COMMAND& command) noexcept
{
    thread_local ExecutorCommand ec(1);
    while (true)
    {
        s_Instance->m_FixedCommandDispatcherSwitch.Wait();
        if (s_Instance->m_ExitThreads)
        {
            break;
        }
        s_Instance->m_FixedCommandDispatcherFinished.Reset();
        // s_Instance->m_Messenger.AssignAndDispatch(command, std::chrono::duration_cast<std::chrono::seconds>(
                                                            //    std::chrono::system_clock::now().time_since_epoch())
                                                            //    .count());
        ec.Set(command, std::time(nullptr));
        s_Instance->m_Messenger.Dispatch(ec);
        s_Instance->m_FixedCommandDispatcherFinished.Set();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    Console::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 m_DispatchFixedCommand 退出。");
    s_Instance->m_ThreadExitEvent.Set();
}