#include <algorithm>
#include <chrono>
#include <executor/core/Native/channel.hpp>
#include <executor/core/Native/multiplexer.hpp>
#include <stdexcept>
#include <system_error>

Multiplexer::Multiplexer() {
  port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
  if (port_ == nullptr) {
    throw std::system_error(GetLastError(), std::system_category(),
                            "CreateIoCompletionPort");
  }
}

Multiplexer::~Multiplexer() { CloseHandle(port_); }

void Multiplexer::AddChannel(Channel* channel) {
  if (channel == nullptr || !channels_.insert(channel).second) {
    throw std::invalid_argument("channel is null or already registered");
  }
}

void Multiplexer::DeleteChannel(Channel* channel) { channels_.erase(channel); }

HANDLE Multiplexer::GetHandle() const { return port_; }

std::vector<std::int64_t> Multiplexer::Wait(std::int32_t timeout) {
  if (timeout < -1) {
    throw std::invalid_argument(
        "timeout must be -1 or nonnegative milliseconds");
  }
  const auto start = std::chrono::steady_clock::now();
  std::vector<std::int64_t> ready;
  for (;;) {
    bool pending = false;
    for (const auto* channel : channels_) {
      pending = pending || channel->IsPending();
    }
    if (!pending) {
      return ready;
    }
    // Bound each wait so a callback posting error is surfaced by the runtime.
    DWORD wait = 100;
    if (!ready.empty()) {
      wait = 0;
    } else if (timeout >= 0) {
      const auto elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::steady_clock::now() - start)
              .count();
      wait = static_cast<DWORD>(std::min<std::int64_t>(
          100, std::max<std::int64_t>(0, timeout - elapsed)));
    }
    DWORD bytes = 0;
    ULONG_PTR key = 0;
    OVERLAPPED* overlapped = nullptr;
    if (!GetQueuedCompletionStatus(port_, &bytes, &key, &overlapped, wait)) {
      const DWORD error = GetLastError();
      if (error != WAIT_TIMEOUT) {
        throw std::system_error(error, std::system_category(),
                                "GetQueuedCompletionStatus");
      }
      for (const auto* channel : channels_) {
        channel->CheckError();
      }
      if (!ready.empty() || timeout == 0 ||
          (timeout > 0 && std::chrono::steady_clock::now() - start >=
                              std::chrono::milliseconds(timeout))) {
        return ready;
      }
      continue;
    }
    auto* channel = reinterpret_cast<Channel*>(key);
    if (!channels_.contains(channel)) {
      throw std::runtime_error("IOCP returned an unregistered channel");
    }
    auto completed = channel->Complete();
    ready.insert(ready.end(), completed.begin(), completed.end());
  }
}