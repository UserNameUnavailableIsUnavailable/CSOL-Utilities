#pragma once

#include <cstdint>
#include <executor/native/types.hpp>
#include <executor/native/waitable.hpp>
#include <queue>
#include <unordered_set>
#include <vector>

class Multiplexer;

class Channel {
 public:
  explicit Channel(ChannelType type);
  virtual ~Channel() = default;
  Channel(const Channel&) = delete;
  Channel& operator=(const Channel&) = delete;
  virtual std::vector<std::int64_t> Complete() = 0;
  virtual bool IsPending() const = 0;
  virtual void CheckError() const = 0;

 protected:
  const ChannelType type_;
};

class TimerChannel final : public Channel {
 public:
  explicit TimerChannel(Multiplexer& multiplexer);
  ~TimerChannel() override;
  void Sleep(std::int64_t task_id, std::chrono::milliseconds delay);
  std::vector<std::int64_t> Complete() override;
  bool IsPending() const override;
  void CheckError() const override;

 private:
  struct Entry {
    std::chrono::steady_clock::time_point due;
    std::int64_t task_id;
  };
  struct Later {
    bool operator()(const Entry& left, const Entry& right) const;
  };
  Multiplexer& multiplexer_;
  Timer timer_;
  std::priority_queue<Entry, std::vector<Entry>, Later> tasks_;
  std::unordered_set<std::int64_t> task_ids_;
};
