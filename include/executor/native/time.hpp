#pragma once

#include <chrono>
#include <executor/native/dll.hpp>

class Time {
 public:
  int GetMonotonicTimestamp(lua_State* state) const;
  int GetSystemTimestamp(lua_State* state) const;

 private:
  std::chrono::steady_clock::time_point monotonic_origin_ =
      std::chrono::steady_clock::now();
};
