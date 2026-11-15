#pragma once

#include <executor/native/keyboard.hpp>
#include <executor/native/mouse.hpp>
#include <executor/native/time.hpp>
#include <memory>

class AsyncRuntime;

struct NativeStorage {
  NativeStorage();
  ~NativeStorage();
  Keyboard keyboard;
  Mouse mouse;
  Time time;
  std::unique_ptr<AsyncRuntime> runtime;
  lua_State* runtime_owner = nullptr;
};

inline constexpr char kNativeStorageRegistryKey[] = "executor.Native.Storage";

NativeStorage& GetStorage(lua_State* state);
// The hook-owned instance cannot be replaced after initialization.
void SetStorage(lua_State* state, NativeStorage* storage);
