#pragma once

class TimerChannel {
public:
    TimerChannel(std::uintptr_t handle);
    ~TimerChannel() noexcept;
    std::uintptr_t handle() const { return handle_; }
private:
    std::uintptr_t handle_;
};