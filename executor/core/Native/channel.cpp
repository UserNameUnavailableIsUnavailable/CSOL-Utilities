#include <executor/core/Native/channel.hpp>
#include <executor/core/Native/multiplexer.hpp>
#include <new>
#include <stdexcept>

Channel::Channel(ChannelType type) : type_(type) {}

TimerChannel::TimerChannel(Multiplexer& multiplexer)
    : Channel(ChannelType::kTimer),
      multiplexer_(multiplexer),
      timer_(multiplexer.GetHandle(), reinterpret_cast<ULONG_PTR>(this)) {
  multiplexer_.AddChannel(this);
}

TimerChannel::~TimerChannel() {
  timer_.Disarm();
  multiplexer_.DeleteChannel(this);
}

bool TimerChannel::Later::operator()(const Entry& left,
                                     const Entry& right) const {
  return left.due > right.due;
}

void TimerChannel::Sleep(std::int64_t task_id,
                         std::chrono::milliseconds delay) {
  if (task_ids_.contains(task_id)) {
    throw std::invalid_argument("task already has a pending timer");
  }
  const auto due = std::chrono::steady_clock::now() + delay;
  task_ids_.insert(task_id);
  try {
    tasks_.push({due, task_id});
  } catch (const std::bad_alloc&) {
    task_ids_.erase(task_id);
    throw;
  }
  timer_.Arm(tasks_.top().due);
}

std::vector<std::int64_t> TimerChannel::Complete() {
  timer_.CheckError();
  std::vector<std::int64_t> ready;
  const auto now = std::chrono::steady_clock::now();
  while (!tasks_.empty() && tasks_.top().due <= now) {
    ready.push_back(tasks_.top().task_id);
    task_ids_.erase(tasks_.top().task_id);
    tasks_.pop();
  }
  if (!tasks_.empty()) {
    timer_.Arm(tasks_.top().due);
  }
  return ready;
}

bool TimerChannel::IsPending() const { return !tasks_.empty(); }

void TimerChannel::CheckError() const { timer_.CheckError(); }
