#pragma once

#include <executor/native/keyboard.hpp>
#include <executor/native/mouse.hpp>
#include <executor/native/time.hpp>

struct NativeStorage {
  Keyboard keyboard;
  Mouse mouse;
  Time time;
};

inline constexpr char kNativeStorageRegistryKey[] = "executor.Native.Storage";

NativeStorage& GetStorage(lua_State* state);
// The hook-owned instance cannot be replaced after initialization.
void SetStorage(lua_State* state, NativeStorage* storage);
