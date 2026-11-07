#pragma once

class Timer {
public:
    Timer();
    ~Timer() noexcept;
private:
    std::uintptr_t handle_;
    std::thread thread_;
};