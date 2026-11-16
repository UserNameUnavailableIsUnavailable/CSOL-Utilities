#pragma once

#include <Windows.h>

#include <atomic>
#include <chrono>

class Waitable {
 public:
  virtual ~Waitable() = default;
  virtual void Arm(std::chrono::steady_clock::time_point due) = 0;
  virtual void Disarm() noexcept = 0;
};

class Timer final : public Waitable {
 public:
  Timer(HANDLE port, ULONG_PTR key);
  ~Timer() override;
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;
  void Arm(std::chrono::steady_clock::time_point due) override;
  void Disarm() noexcept override;
  void CheckError() const;

 private:
  static void CALLBACK Complete(PTP_CALLBACK_INSTANCE instance, void* context,
                                PTP_TIMER timer);
  HANDLE port_;
  ULONG_PTR key_;
  PTP_TIMER timer_ = nullptr;
  std::atomic<DWORD> error_{ERROR_SUCCESS};
};
