#pragma once

#include "ModuleBase.hpp"
#include "Signal.hpp"
#include <Windows.h>
#include <array>
#include <deque>
#include <memory>
#include <minwinbase.h>
#include <synchapi.h>

namespace CSOL_Utilities
{
    // Win32 API 实现的模块
    class Win32ModuleBase : public ModuleBase
    {
    public:
        explicit Win32ModuleBase(std::string name, BOOL bCreateSuspended = FALSE, UINT uWorkIntervalInMilliseconds = 1000);
        Win32ModuleBase(Win32ModuleBase&) = delete;
        ~Win32ModuleBase() noexcept;
        HANDLE get_worker_handle() const noexcept override { return reinterpret_cast<HANDLE>(m_hWorker); }
        virtual void suspend() noexcept override;
        virtual void resume() noexcept override;
        virtual void retire() noexcept override;
        virtual bool bootstrap() noexcept override;
        virtual void send(unsigned int id, ModuleMessage&& module_message) override;
        virtual void send(unsigned int id, ModuleMessage&& module_message, unsigned int timeout_milliseconds) override;
        virtual void receive(ModuleMessage& module_message) override;
        virtual void receive(ModuleMessage& module_message, unsigned int timeout_milliseconds) override;
    protected:
        virtual void entry() noexcept override; // 打工人入职（线程入口函数）
        virtual bool work() noexcept override = 0;
    private:
        uintptr_t m_hWorker = 0; // 打工人线程，此句柄不需要手动关闭
        CRITICAL_SECTION m_WorkerStateLock; // 锁定工人当前状态，进行原子操作
        CONDITION_VARIABLE m_CanWorkerSuspend; // 打工人是否可以停工
        CONDITION_VARIABLE m_ShouldWorkerResume; // 打工人是否应该复工
        BOOL m_bTaskFinishedByWorker = TRUE; // 工人是否完成任务
        // INT m_bTaskAssignedToWorker = TRUE; // 工人是否接到任务
        UINT m_uHolidayCount; // 停工计数，每次调用 suspend 将此值增加 1
        BOOL m_bWorkerRetired = FALSE; // 打工人是否退休
        BOOL m_bWorkerDead = FALSE; // 打工人是否死亡
        UINT m_uWorkerIntervalInMilliseconds = 1000; // 工作间隙
    };
}
