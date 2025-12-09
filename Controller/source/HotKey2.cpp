#include "HotKey2.hpp"

#include <Windows.h>
#include <atomic>

#include "Utilities.hpp"
#include "Console.hpp"
#include "Exception.hpp"

namespace CSOL_Utilities
{
HotKey2 CreateHotKey(const Modifiers& modifiers, std::uint32_t vk, std::uintptr_t window_handle)
{
    std::atomic_uint32_t counter = 0;
    std::uint32_t mod = 0;
    std::string desc = "{ ";
    if (modifiers.win)
    {
        desc += "Win ";
        mod |= MOD_WIN;
    }
    if (modifiers.alt)
    {
        desc += "Alt ";
        mod |= MOD_ALT;
    }
    if (modifiers.ctrl)
    {
        desc += "Ctrl ";
        mod |= MOD_CONTROL;
    }
    if (modifiers.shift)
    {
        desc += "Shift ";
        mod |= MOD_SHIFT;
    }
    if (modifiers.no_repeat)
    {
        mod |= MOD_NOREPEAT;
    }
    desc += "}";
    HWND hWnd = reinterpret_cast<HWND>(window_handle);
    UINT id = ++counter;
    auto ok = RegisterHotKey(hWnd, id, mod, vk);
    if (!ok)
    {
        auto err_msg = Translate("ERROR_RegisterHotKey@2", desc, GetLastError());
        throw Exception(err_msg);
    }
    auto info_msg = Translate("INFO_RegisterHotKey@1", desc);
    Console::Info(info_msg);
    auto deleter = [hWnd, id]() noexcept
    {
        UnregisterHotKey(hWnd, id);
    };
    return HotKey2(id, std::move(desc), std::move(deleter));
}
} // namespace CSOL_Utilities