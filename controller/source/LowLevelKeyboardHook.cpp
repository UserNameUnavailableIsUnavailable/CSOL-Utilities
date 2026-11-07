#include "LowLevelKeyboardHook.hpp"

#include "Exception.hpp"
#include "Utilities.hpp"

namespace CSOL_Utilities
{
LowLevelKeyboardHook::LowLevelKeyboardHook(HOOKPROC hook_proc, bool defer) : m_HookProc(hook_proc)
{
    if (!defer)
    {
        Install();
    }
}

void LowLevelKeyboardHook::Install()
{
    if (m_bInstalled.load(std::memory_order_acquire))
    {
        return;
    }
    m_hHook = SetWindowsHookExW(WH_KEYBOARD_LL, m_HookProc, nullptr, 0);

    if (!m_hHook)
    {
        throw Exception(Translate("LowLevelKeyboardHook::ERROR_SetWindowsHookExW@1", GetLastError()));
    }

    m_bInstalled.store(true, std::memory_order_release);
}

LowLevelKeyboardHook::~LowLevelKeyboardHook() noexcept
{
    if (m_bInstalled.load(std::memory_order_acquire))
    {
        UnhookWindowsHookEx(m_hHook);
    }
}

LowLevelKeyboardHook::LowLevelKeyboardHook(LowLevelKeyboardHook &&llhk)
{
    std::swap(m_hHook, llhk.m_hHook);
    std::swap(m_HookProc, llhk.m_HookProc);
    auto tmp = m_bInstalled.exchange(llhk.m_bInstalled.load(std::memory_order_acquire), std::memory_order_release);
    llhk.m_bInstalled.store(tmp);
}
} // namespace CSOL_Utilities