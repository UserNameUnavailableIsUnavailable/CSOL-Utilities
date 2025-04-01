#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <json/json.h>
#include "Event.hpp"

namespace CSOL_Utilities
{
    class ExecutorChecker
    {
    public:
        ExecutorChecker(std::wstring process_name);
    private:
        void DebugExecutor() noexcept;
    private:
        std::atomic_bool m_alive_ = true;
        std::wstring m_executor_process_name_; // Executor 名称，一般为 lghub_agent.exe
        Event m_switch_;
    };
};