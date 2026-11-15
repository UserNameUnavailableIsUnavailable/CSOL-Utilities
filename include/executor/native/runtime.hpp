#pragma once

#include <executor/native/channel.hpp>
#include <executor/native/multiplexer.hpp>

class AsyncRuntime {
 public:
  void Sleep(std::int64_t task_id, std::chrono::milliseconds delay);
  std::vector<std::int64_t> Poll(std::int32_t timeout);

 private:
  Multiplexer multiplexer_;
  TimerChannel timer_channel_{multiplexer_};
};
