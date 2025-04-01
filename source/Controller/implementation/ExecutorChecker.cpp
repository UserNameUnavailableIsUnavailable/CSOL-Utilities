#include "ExecutorChecker.hpp"
#include "CSOL_Utilities.hpp"
#include <Windows.h>
#include <cstddef>
#include <debugapi.h>
#include <minwindef.h>
#include <mutex>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <winnt.h>

namespace CSOL_Utilities
{
    void ExecutorChecker::DebugExecutor() noexcept
    {
        DWORD dwExecutorPId = GetProcessIdByName(m_executor_process_name_);
        UniqueHandle hExecutorProcess;
        if (dwExecutorPId)
        {
            hExecutorProcess = OpenProcess(PROCESS_VM_READ | SYNCHRONIZE, FALSE, dwExecutorPId);
        }
        while (true)
        {
            m_switch_.Wait();
            if (!m_alive_) { break; }
            if (WAIT_OBJECT_0 == WaitForSingleObject(hExecutorProcess.get(), 0))
            {

            }
            if (!hExecutorProcess)
            {

            }
        }
        if (dwExecutorPId)
        {
            DebugActiveProcessStop(dwExecutorPId);
        }
    }
};