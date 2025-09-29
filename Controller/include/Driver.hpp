#pragma once

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
    HotKey null_mode_hotkey;
    HotKey normal_idle_mode_hotkey;
    HotKey extended_idle_mode_hotkey;
    HotKey batch_combine_parts_mode_hotkey;
    HotKey batch_purchase_item_mode_hotkey;
    HotKey locate_cursor_mode_hotkey;
};

class Driver
{
  public:
    Driver(std::unique_ptr<DriverHotkeyBindings> driver_hotkey_bindings, std::unique_ptr<IdleEngine> idle_engine,
           std::unique_ptr<CommandDispatcher> command_dispatcher)
        : hotkey_bindings_(std::move(driver_hotkey_bindings)), idle_engine_(std::move(idle_engine)),
          command_dispatcher_(std::move(command_dispatcher)), current_mode_id_(hotkey_bindings_->null_mode_hotkey.Id())
    {
    }
    ~Driver() noexcept;
    void RegisterLowLevelKeyboardHook(LowLevelKeyboardHook llhk);
    void RegisterOptionalModule(std::unique_ptr<Module> module);
    void Boot();
    void Terminate() noexcept;

  private:
    std::vector<LowLevelKeyboardHook> llkh_;                /* 低级键盘钩子列表 */
    std::unique_ptr<DriverHotkeyBindings> hotkey_bindings_; /* 热键绑定 */
    std::vector<std::unique_ptr<Module>> optional_modules_; /* 可选模块列表 */
    std::mutex lock_;                                       /* 修改成员变量时加锁 */
    bool is_frozen_ = false;                                /* 驱动器是否已冻结，冻结后不允许再添加新模块 */
    DWORD win32_thread_id_ = 0;                             /* 记录 Win32 线程 ID */
    void HandleHotKey(WPARAM wParam, LPARAM lParam);
    std::unique_ptr<IdleEngine> idle_engine_;
    std::unique_ptr<CommandDispatcher> command_dispatcher_;
    int current_mode_id_;
};
} // namespace CSOL_Utilities
