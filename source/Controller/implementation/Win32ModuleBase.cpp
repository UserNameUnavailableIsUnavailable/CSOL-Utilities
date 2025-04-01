#include "Win32ModuleBase.hpp"
#include <Windows.h>
#include <cstddef>
#include <processenv.h>
#include <synchapi.h>
#include <process.h>
#include <winerror.h>
#include <winnt.h>

using namespace CSOL_Utilities;

Win32ModuleBase::Win32ModuleBase(std::string name, BOOL bCreateSuspended, unsigned int uWorkIntervalInMilliseconds) :
    ModuleBase(name) // 调用基类构造器，初始化模块名称
{
    InitializeConditionVariable(&m_CanWorkerSuspend);
    InitializeConditionVariable(&m_ShouldWorkerResume);
    InitializeCriticalSection(&m_WorkerStateLock);
    if (bCreateSuspended) // 不使用 _beginthreadex 的 InitFlag 在创建时挂起线程，而是让线程自发挂起
    {
        m_uHolidayCount = 1;
    }
    m_uWorkerIntervalInMilliseconds = uWorkIntervalInMilliseconds;
}

bool Win32ModuleBase::bootstrap() noexcept
{
    m_hWorker = _beginthreadex(
            NULL,
            4096,
            [] (void* self) { static_cast<Win32ModuleBase*>(self)->entry(); return 0u; },
            this,
            0,
            NULL
    );
    return m_hWorker;
}

Win32ModuleBase::~Win32ModuleBase() noexcept
{
    retire(); // 结束线程运行
    WaitForSingleObject(reinterpret_cast<HANDLE>(m_hWorker), INFINITE);
}

void Win32ModuleBase::entry() noexcept
{
    BOOL bError = FALSE;
    while (TRUE)
    {
        // 进入临界区
        EnterCriticalSection(&m_WorkerStateLock);

        while (!(m_uHolidayCount == 0 || m_bWorkerRetired)) // 假日减为 0，或退休
        {
            SleepConditionVariableCS(
                &m_ShouldWorkerResume,
                &m_WorkerStateLock,
                INFINITE
            );
        }

        // 退休
        if (m_bWorkerRetired)
        {
            goto LEAVE_CRITICAL_SECTION_AND_BREAK; // 离开临界区，并结束线程运行
        }
        // 有新任务
        m_bTaskFinishedByWorker = FALSE; // 开始新任务
        LeaveCriticalSection(&m_WorkerStateLock); // 解锁

        // 离开临界区后开始工作
        if (!work()) // 工作过程中出现异常
        {
            EnterCriticalSection(&m_WorkerStateLock);
            m_bWorkerDead = TRUE;
            bError = TRUE;
            goto LEAVE_CRITICAL_SECTION_AND_BREAK; // 离开临界区，并结束线程运行
        }
        
        // 工作完成后，进入临界区，将工作标记为完成
        EnterCriticalSection(&m_WorkerStateLock);
        m_bTaskFinishedByWorker = TRUE;
        goto LEAVE_CRITICAL_SECTION_AND_CONTINUE;

    LEAVE_CRITICAL_SECTION_AND_CONTINUE: // 离开临界区，准备下一轮运行
        LeaveCriticalSection(&m_WorkerStateLock);
        WakeConditionVariable(&m_CanWorkerSuspend);
        Sleep(m_uWorkerIntervalInMilliseconds);
        continue;
    LEAVE_CRITICAL_SECTION_AND_BREAK: // 离开临界区，结束线程运行
        LeaveCriticalSection(&m_WorkerStateLock);
        WakeConditionVariable(&m_CanWorkerSuspend);
        break;
    }
    _endthreadex(0); // 结束线程运行
}

void Win32ModuleBase::suspend() noexcept
{
    EnterCriticalSection(&m_WorkerStateLock);

    while (!(m_bTaskFinishedByWorker || m_bWorkerRetired || m_bWorkerDead)) // 任务完成、退休、死亡
    {
        SleepConditionVariableCS(&m_CanWorkerSuspend, &m_WorkerStateLock, INFINITE);
    }

    m_uHolidayCount++; // 假日加 1

    LeaveCriticalSection(&m_WorkerStateLock);
}

void Win32ModuleBase::resume() noexcept
{
    EnterCriticalSection(&m_WorkerStateLock);
    m_uHolidayCount--; // 假日减 1
    LeaveCriticalSection(&m_WorkerStateLock);
    WakeConditionVariable(&m_ShouldWorkerResume); // 让打工人复工
}

void Win32ModuleBase::retire() noexcept
{
    EnterCriticalSection(&m_WorkerStateLock);
    m_bWorkerRetired = TRUE; // 让工人退休
    LeaveCriticalSection(&m_WorkerStateLock);
    WakeConditionVariable(&m_ShouldWorkerResume); // 让工人复工之后收拾东西滚蛋
}