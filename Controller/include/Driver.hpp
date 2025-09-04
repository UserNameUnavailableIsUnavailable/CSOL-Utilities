﻿#pragma once

#include "CSOBannerSuppressor.hpp"
#include "CommandDispatcher.hpp"
#include "HotKey.hpp"
#include "IdleEngine.hpp"
#include "LowLevelKeyboardHook.hpp"
#include "Module.hpp"

namespace CSOL_Utilities
{
    struct DriverHotkeyBindings
    {
        HotKey hkNULL;
        HotKey hkNormalIdle;
        HotKey hkExtendedIdle;
        HotKey hkBatchCombineParts;
        HotKey hkBatchPurchaseItem;
		HotKey hkLocateCursor;
    };

    class Driver
    {
    public:
        Driver(std::unique_ptr<DriverHotkeyBindings> driver_hotkey_bindings, std::unique_ptr<IdleEngine> idle_engine, std::unique_ptr<CommandDispatcher> command_dispatcher) :
            m_DriverHotKeyBindings(std::move(driver_hotkey_bindings)),
            m_IdleEngine(std::move(idle_engine)),
            m_CommandDispatcher(std::move(command_dispatcher)),
            m_CurrentModeId(m_DriverHotKeyBindings->hkNULL.Id())
        {
        }
        ~Driver() noexcept;
        void RegisterLowLevelKeyboardHook(LowLevelKeyboardHook llhk);
        void RegisterOptionalModule(std::unique_ptr<Module> module);
        void Launch();
        void Terminate() noexcept;
    private:
        std::vector<LowLevelKeyboardHook> m_LowLevelKeyboardHooks;
        std::mutex m_FrozenLock;
        bool m_Frozen = false;
        std::vector<std::unique_ptr<Module>> m_OptionalModules;
        std::atomic<DWORD> m_dwThreadId = 0;
        void HandleHotKey(WPARAM wParam, LPARAM lParam);
        std::unique_ptr<IdleEngine> m_IdleEngine;
        std::unique_ptr<CommandDispatcher> m_CommandDispatcher;
        std::unique_ptr<CSOBannerSuppressor> m_CSOBannerKiller;
        std::unique_ptr<DriverHotkeyBindings> m_DriverHotKeyBindings;
        int m_CurrentModeId;
    };
}
