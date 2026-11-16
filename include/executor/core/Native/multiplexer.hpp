#pragma once

#include <Windows.h>

#include <cstdint>
#include <unordered_set>
#include <vector>

class Channel;

class Multiplexer {
 public:
  Multiplexer();
  ~Multiplexer();
  Multiplexer(const Multiplexer&) = delete;
  Multiplexer& operator=(const Multiplexer&) = delete;
  void AddChannel(Channel* channel);
  void DeleteChannel(Channel* channel);
  std::vector<std::int64_t> Wait(std::int32_t timeout);
  HANDLE GetHandle() const;

 private:
  HANDLE port_ = nullptr;
  std::unordered_set<Channel*> channels_;
};
