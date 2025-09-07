#pragma once

namespace CSOL_Utilities
{
    class Module
    {
    public:
        virtual void Boot() = 0; /* 启动模块，创建线程等资源（线程创建后处于挂起状态） */
        virtual void Resume() = 0; /* 恢复模块运行 */
        virtual void Suspend() = 0; /* 挂起模块运行 */
        virtual void Terminate() noexcept = 0; /* 终止模块，释放线程等资源 */
        virtual ~Module() = default;
    };
}