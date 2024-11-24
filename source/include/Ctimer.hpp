#include "CException.hpp"
#include <Windows.h>
#include <chrono>
#include <ctime>
#include <errhandlingapi.h>
#include <handleapi.h>
#include <synchapi.h>
#include <tuple>
#include <utility>
#include <winnt.h>

namespace CSOL_Utilities
{
class CDeadlineTimer
{
public:
    CDeadlineTimer()
    {
        hTimer = CreateWaitableTimerW(nullptr, true, nullptr);
        if (!hTimer)
        {
            throw CException("创建计时器失败，错误代码：%lu。", GetLastError());
        }
    }
    ~CDeadlineTimer()
    {
        CloseHandle(hTimer);
    }
    void Set(std::chrono::steady_clock::time_point deadline, void (*f) (void))
    {
        auto now = std::chrono::steady_clock::now();
        auto duration = deadline - now;
        auto due = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        m_Due.QuadPart = 10 /* to 100 ns */ * 1000 /* μs */ * 1000 /* ms */ * due;
        SetWaitableTimer(hTimer, &m_Due, 0, f, nullptr, false);
    }
private:
    HANDLE hTimer = nullptr;
    LARGE_INTEGER m_Due;
};
}