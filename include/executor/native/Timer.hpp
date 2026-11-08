#pragma once

#include <cstddef>
#include <thread>

class Timer {
public:
    Timer();
    ~Timer() noexcept;
private:
    std::uintptr_t handle_;
};