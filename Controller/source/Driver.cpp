#include "pch.hpp"

#include "Driver.hpp"
#include "Command.hpp"
#include "Console.hpp"
#include "Exception.hpp"
#include "LowLevelKeyboardHook.hpp"

namespace CSOL_Utilities
{
    Driver::~Driver() noexcept
    {
    }

    void Driver::Terminate() noexcept
    {
        if (m_dwThreadId.load(std::memory_order_acquire) != 0)
        {
            PostThreadMessageW(m_dwThreadId, WM_QUIT, 0, 0);
        }
    }

    void Driver::RegisterLowLevelKeyboardHook(LowLevelKeyboardHook llhk)
    {
        std::lock_guard lk(m_FrozenLock);
        if (m_Frozen)
        {
            throw Exception("Driver::ERROR_DriverIsFrozen");
        }
        m_LowLevelKeyboardHooks.emplace_back(std::move(llhk));
    }

    void Driver::HandleHotKey(WPARAM wParam, LPARAM lParam)
    {
        UINT uId = wParam;
        UINT uModifiers = lParam;

        thread_local std::function<void ()> disable_current_mode = [] {};
    
        if (m_CurrentModeId == uId) { return; }
        
        disable_current_mode();

        if (uId == m_DriverHotKeyBindings->hkNULL.Id())
        {
            Console::Info(Translate("Driver::INFO_SwitchToNullMode"));
            disable_current_mode = [] {};
        }
        else if (uId == m_DriverHotKeyBindings->hkNormalIdle.Id())
        {
            m_IdleEngine->SetIdleMode(IDLE_MODE::IM_DEFAULT);
            m_IdleEngine->Resume();
            disable_current_mode = [this] {
                m_IdleEngine->Suspend();
                Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP);
                m_IdleEngine->ResetStateAfterSwitchMode();
            };
            Console::Info(Translate("Driver::INFO_SwitchToDefaultIdleMode"));
        }
        else if (uId == m_DriverHotKeyBindings->hkExtendedIdle.Id())
        {
            m_IdleEngine->SetIdleMode(IDLE_MODE::IM_EXTENDED);
            m_IdleEngine->Resume();
            disable_current_mode = [this] {
                m_IdleEngine->Suspend();
                Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP);
                m_IdleEngine->ResetStateAfterSwitchMode();
            };
            Console::Info(Translate("Driver::INFO_SwitchToExtendedIdleMode"));
        }
        else if (uId == m_DriverHotKeyBindings->hkBatchCombineParts.Id())
        {
            Command::Set(Command::TYPE::CMD_BATCH_COMBINE_PARTS, Command::CMD_REPEATABLE | Command::CMD_AUTO_REFRESH);
            disable_current_mode = [] { Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP); };
            Console::Info(Translate("Driver::INFO_SwitchToBatchCombinePartsMode"));
        }
        else if (uId == m_DriverHotKeyBindings->hkBatchPurchaseItem.Id())
        {
            Command::Set(Command::TYPE::CMD_BATCH_PURCHASE_ITEM, Command::CMD_REPEATABLE | Command::CMD_AUTO_REFRESH);
            disable_current_mode = [] { Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP); };
            Console::Info(Translate("Driver::INFO_SwitchToBatchPurchaseItemMode"));
        }
		else if (uId == m_DriverHotKeyBindings->hkLocateCursor.Id())
		{
			Command::Set(Command::TYPE::CMD_LOCATE_CURSOR, Command::CMD_REPEATABLE | Command::CMD_AUTO_REFRESH);
			disable_current_mode = [] { Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP); };
			Console::Info(Translate("Driver::INFO_SwitchToLocateCursorMode"));
		}
        m_CurrentModeId = uId;
    }

    void Driver::RegisterOptionalModule(std::unique_ptr<Module> module)
    {
        std::lock_guard lk(m_FrozenLock);
        if (m_Frozen)
        {
            throw Exception("Driver::ERROR_DriverIsFrozen");
        }
        m_OptionalModules.emplace_back(std::move(module));
    }

    void Driver::Launch()
    {
        {
            std::lock_guard lk(m_FrozenLock);
            m_Frozen = true; /* 锁定驱动器，不允许再添加新模块 */
        }

        /* 与调用线程绑定 */
        m_dwThreadId.store(GetCurrentThreadId(), std::memory_order_release);

        /* 注册热键 */
        m_DriverHotKeyBindings->hkNULL.Register();
		m_DriverHotKeyBindings->hkNormalIdle.Register();
		m_DriverHotKeyBindings->hkExtendedIdle.Register();
		m_DriverHotKeyBindings->hkBatchCombineParts.Register();
		m_DriverHotKeyBindings->hkBatchPurchaseItem.Register();
		m_DriverHotKeyBindings->hkLocateCursor.Register();

        /* 应用注册的键盘钩子 */
        for (auto& llhk : m_LowLevelKeyboardHooks)
        {
            llhk.Install();
        }

        /* 启动命令指派器 */
        m_CommandDispatcher->Resume();

        /* 启动可选模块 */
        for (auto& i : m_OptionalModules)
        {
            i->Resume();
        }

        MSG msg;
        BOOL bRet;
        while ((bRet = GetMessageW(&msg, nullptr, 0, 0)) != 0)
        {
            if (bRet == -1)
            {
                // 目前 hWnd 为 nullptr，所以不可能出现返回 -1 的情况
            }
            switch (msg.message)
            {
            case WM_HOTKEY: HandleHotKey(msg.wParam, msg.lParam); break;
            default: break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        m_dwThreadId.store(0, std::memory_order_release);
    }
}
