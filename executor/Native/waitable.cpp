#include <algorithm>
#include <executor/native/waitable.hpp>
#include <system_error>

Timer::Timer(HANDLE port, ULONG_PTR key) : port_(port), key_(key) {
  timer_ = CreateThreadpoolTimer(Complete, this, nullptr);
  if (timer_ == nullptr) {
    throw std::system_error(GetLastError(), std::system_category(),
                            "CreateThreadpoolTimer");
  }
}

Timer::~Timer() {
  Disarm();
  CloseThreadpoolTimer(timer_);
}

void Timer::Arm(std::chrono::steady_clock::time_point due) {
  CheckError();
  const auto delay =
      (std::max)(std::chrono::duration_cast<std::chrono::nanoseconds>(
                     due - std::chrono::steady_clock::now()),
                 std::chrono::nanoseconds(100));
  LARGE_INTEGER relative;
  relative.QuadPart = -std::max<LONGLONG>(1, delay.count() / 100);
  FILETIME time{relative.LowPart, static_cast<DWORD>(relative.HighPart)};
  SetThreadpoolTimer(timer_, &time, 0, 0);
}

void Timer::Disarm() noexcept {
  SetThreadpoolTimer(timer_, nullptr, 0, 0);
  WaitForThreadpoolTimerCallbacks(timer_, TRUE);
}

void Timer::CheckError() const {
  const DWORD error = error_.load();
  if (error != ERROR_SUCCESS) {
    throw std::system_error(error, std::system_category(),
                            "PostQueuedCompletionStatus");
  }
}

void CALLBACK Timer::Complete(PTP_CALLBACK_INSTANCE, void* context, PTP_TIMER) {
  auto* timer = static_cast<Timer*>(context);
  if (!PostQueuedCompletionStatus(timer->port_, 0, timer->key_, nullptr)) {
    timer->error_.store(GetLastError());
  }
}
