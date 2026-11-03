#include "Driver.hpp"

#include "Command.hpp"
#include "Console.hpp"
#include "Exception.hpp"
#include "LowLevelKeyboardHook.hpp"

namespace CSOL_Utilities
{
Driver::~Driver() noexcept
{
    Terminate();
}

void Driver::Terminate() noexcept
{
    std::lock_guard lk(lock_);
    if (win32_thread_id_ != 0) // 线程正在运行，发送退出消息
    {
        PostThreadMessageW(win32_thread_id_, WM_QUIT, 0, 0);
        win32_thread_id_ = 0;
    }
}

void Driver::RegisterLowLevelKeyboardHook(LowLevelKeyboardHook llhk)
{
    std::lock_guard lk(lock_);
    if (is_frozen_)
    {
        throw Exception("Driver::ERROR_DriverIsFrozen");
    }
    llkh_.emplace_back(std::move(llhk));
}

void Driver::HandleHotKey(WPARAM wParam, LPARAM lParam)
{
    UINT id = wParam;
    UINT modifier = lParam;

    thread_local std::function<void()> disable_current_mode = [] {};

    if (current_mode_id_ == id)
    {
        return;
    }

    disable_current_mode();

    if (id == hotkey_bindings_->null_mode_hotkey.Id())
    {
        Console::Info(Translate("Driver::INFO_SwitchToNullMode"));
        disable_current_mode = [] {};
    }
    else if (id == hotkey_bindings_->normal_idle_mode_hotkey.Id())
    {
        idle_engine_->SetIdleMode(IDLE_MODE::DEFAULT);
        idle_engine_->Resume();
        disable_current_mode = [this] {
            idle_engine_->Suspend();
            Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP);
            idle_engine_->ResetAfterSwitchMode();
        };
        Console::Info(Translate("Driver::INFO_SwitchToDefaultIdleMode"));
    }
    else if (id == hotkey_bindings_->extended_idle_mode_hotkey.Id())
    {
        idle_engine_->SetIdleMode(IDLE_MODE::EXTENDED);
        idle_engine_->Resume();
        disable_current_mode = [this] {
            idle_engine_->Suspend();
            Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP);
            idle_engine_->ResetAfterSwitchMode();
        };
        Console::Info(Translate("Driver::INFO_SwitchToExtendedIdleMode"));
    }
    else if (id == hotkey_bindings_->batch_combine_parts_mode_hotkey.Id())
    {
        Command::Set(Command::TYPE::CMD_BATCH_COMBINE_PARTS, Command::CMD_REPEATABLE | Command::CMD_AUTO_REFRESH);
        disable_current_mode = [] { Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP); };
        Console::Info(Translate("Driver::INFO_SwitchToBatchCombinePartsMode"));
    }
    else if (id == hotkey_bindings_->batch_purchase_item_mode_hotkey.Id())
    {
        Command::Set(Command::TYPE::CMD_BATCH_PURCHASE_ITEM, Command::CMD_REPEATABLE | Command::CMD_AUTO_REFRESH);
        disable_current_mode = [] { Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP); };
        Console::Info(Translate("Driver::INFO_SwitchToBatchPurchaseItemMode"));
    }
    else if (id == hotkey_bindings_->locate_cursor_mode_hotkey.Id())
    {
        Command::Set(Command::TYPE::CMD_LOCATE_CURSOR, Command::CMD_REPEATABLE | Command::CMD_AUTO_REFRESH);
        disable_current_mode = [] { Command::Set(Command::TYPE::CMD_NOP, Command::CMD_ZERO_TIMESTAMP); };
        Console::Info(Translate("Driver::INFO_SwitchToLocateCursorMode"));
    }
    current_mode_id_ = id;
}

void Driver::RegisterOptionalModule(std::unique_ptr<Module> module)
{
    std::lock_guard lk(lock_);
    if (is_frozen_)
    {
        throw Exception("Driver::ERROR_DriverIsFrozen");
    }
    optional_modules_.emplace_back(std::move(module));
}

void Driver::Boot()
{
    {
        std::lock_guard lk(lock_);
        is_frozen_ = true; /* 锁定驱动器，不允许再添加新模块 */
    }

    /* 与调用线程绑定 */
    win32_thread_id_ = GetCurrentThreadId();

    /* 注册热键 */
    hotkey_bindings_->null_mode_hotkey.Register();
    hotkey_bindings_->normal_idle_mode_hotkey.Register();
    hotkey_bindings_->extended_idle_mode_hotkey.Register();
    hotkey_bindings_->batch_combine_parts_mode_hotkey.Register();
    hotkey_bindings_->batch_purchase_item_mode_hotkey.Register();
    hotkey_bindings_->locate_cursor_mode_hotkey.Register();

    /* 应用注册的键盘钩子 */
    for (auto &llhk : llkh_)
    {
        llhk.Install();
    }

    /* 启动命令指派器 */
    command_dispatcher_->Boot();
    command_dispatcher_->Resume(); // 启动后立即运行
    /* 启动挂机引擎 */
    idle_engine_->Boot(); // 启动后按需运行

    /* 启动可选模块 */
    for (auto &i : optional_modules_)
    {
        i->Boot();
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
        case WM_HOTKEY:
            HandleHotKey(msg.wParam, msg.lParam);
            break;
        default:
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}
} // namespace CSOL_Utilities
