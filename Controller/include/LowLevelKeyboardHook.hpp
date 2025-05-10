#pragma once

#include <Windows.h>
#include <atomic>

namespace CSOL_Utilities
{
    class LowLevelKeyboardHook
    {
    public:
        LowLevelKeyboardHook(HOOKPROC hook_proc, bool defer = true);
        LowLevelKeyboardHook(const LowLevelKeyboardHook&) = delete;
        LowLevelKeyboardHook(LowLevelKeyboardHook&&);
        ~LowLevelKeyboardHook() noexcept;
        void Install();
    private:
        HOOKPROC m_HookProc{ nullptr };
        std::atomic_bool m_bInstalled{ false };
        HHOOK m_hHook{ nullptr };
    };
}