#include <executor/core/Native/runtime.hpp>
#include <stdexcept>

void AsyncRuntime::Sleep(std::int64_t task_id,
                         std::chrono::milliseconds delay) {
  if (task_id < 0 || delay.count() < 0 || delay.count() > INT32_MAX) {
    throw std::invalid_argument(
        "expected a nonnegative task id and delay from 0 to 2147483647 ms");
  }
  timer_channel_.Sleep(task_id, delay);
}

std::vector<std::int64_t> AsyncRuntime::Poll(std::int32_t timeout) {
  timer_channel_.CheckError();
  return multiplexer_.Wait(timeout);
}
