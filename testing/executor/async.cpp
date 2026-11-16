#include <Windows.h>
#include <gtest/gtest.h>
#include <lghubex/dll/hook.h>

#include <chrono>
#include <executor/core/Native/global_storage.hpp>
#include <executor/core/Native/runtime.hpp>
#include <filesystem>
#include <string>
#include <thread>

namespace {

void ConfigureNativePath(lua_State* state) {
  lua_getglobal(state, "package");
  lua_getfield(state, -1, "cpath");
  const char* existing = lua_tostring(state, -1);
  std::string path = std::string(TEST_PROJECT_ROOT) + "\\?.dll";
  if (existing != nullptr) {
    path += ";";
    path += existing;
  }
  lua_pop(state, 1);
  lua_pushlstring(state, path.data(), path.size());
  lua_setfield(state, -2, "cpath");
  lua_pop(state, 1);
}

}  // namespace

class AsyncTest : public ::testing::Test {
 protected:
  void SetUp() override {
    state_ = luaL_newstate();
    ASSERT_NE(state_, nullptr);
    luaL_openlibs(state_);
    InstallNativeStorage(state_);
    ConfigureNativePath(state_);
    luaL_getsubtable(state_, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
    const std::filesystem::path root(TEST_PROJECT_ROOT);
    for (const auto* name : {"Task", "Scheduler", "Runtime"}) {
      const auto path =
          root / "executor" / "Core" / (std::string(name) + ".lua");
      ASSERT_EQ(luaL_loadfile(state_, path.string().c_str()), LUA_OK)
          << lua_tostring(state_, -1);
      const auto module = std::string("executor.core.") + name;
      lua_setfield(state_, -2, module.c_str());
    }
    lua_pop(state_, 1);
    Run(R"lua(
      PressKey = function() end
      ReleaseKey = function() end
      PressAndReleaseKey = function() end
      IsKeyLockOn = function() return false end
      PressMouseButton = function() end
      ReleaseMouseButton = function() end
      PressAndReleaseMouseButton = function() end
      GetMousePosition = function() return 0, 0 end
      MoveMouseToVirtual = function() end
      MoveMouseRelative = function() end
      MoveMouseWheel = function() end
      Native = require("executor.core.Native")
      Task = require("executor.core.Task")
      Scheduler = require("executor.core.Scheduler")
      Runtime = require("executor.core.Runtime")
      function spawn(fn)
        local task = Task.create(fn)
        Scheduler.prepare(task)
        Scheduler.submit(task:get_id())
        return task
      end
      function drain()
        while Scheduler.run() do end
      end
    )lua");
  }

  void TearDown() override {
    if (state_ != nullptr) {
      lua_close(state_);
    }
  }

  void Run(const char* script) {
    ASSERT_EQ(luaL_dostring(state_, script), LUA_OK)
        << (lua_tostring(state_, -1) ? lua_tostring(state_, -1) : "");
  }

  lua_State* state_ = nullptr;
};

TEST_F(AsyncTest, NativeTimersBlockAndReportEachIdOnce) {
  const auto start = std::chrono::steady_clock::now();
  Run(R"lua(
    assert(#Native.Runtime_poll(0) == 0)
    Native.Runtime_sleep(1, 30)
    Native.Runtime_sleep(2, 60)
    local seen, count = {}, 0
    while count < 2 do
      for _, id in ipairs(Native.Runtime_poll()) do
        assert(not seen[id])
        seen[id] = true
        count = count + 1
      end
    end
    assert(seen[1] and seen[2])
    assert(#Native.Runtime_poll(0) == 0)
  )lua");
  EXPECT_GE(std::chrono::steady_clock::now() - start,
            std::chrono::milliseconds(60));
}

TEST_F(AsyncTest, NativePollDrainsMultipleReadyTimersAndSupportsTimeout) {
  Run(R"lua(
    for id = 1, 20 do Native.Runtime_sleep(id, 0) end
  )lua");
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  Run(R"lua(
    local ids = Native.Runtime_poll()
    assert(#ids == 20)
    local seen = {}
    for _, id in ipairs(ids) do assert(not seen[id]); seen[id] = true end
    Native.Runtime_sleep(21, 1000)
    assert(#Native.Runtime_poll(2) == 0)
    Native.Runtime_close()
    assert(not pcall(Native.Runtime_poll, 0))
    Native.Runtime_open()
    assert(#Native.Runtime_poll(0) == 0)
  )lua");
}

TEST_F(AsyncTest, ValidatesNativeOperationsWithoutPartialRegistration) {
  Run(R"lua(
    for _, value in ipairs({-1, 1.5, "1", false, {}}) do
      assert(not pcall(Native.Runtime_sleep, value, 10))
      assert(not pcall(Native.Runtime_sleep, 1, value))
    end
    assert(not pcall(Native.Runtime_sleep, 1, 2147483648))
    assert(not pcall(Native.Runtime_sleep, 1))
    assert(not pcall(Native.Runtime_sleep, 1, 2, 3))
    assert(#Native.Runtime_poll(0) == 0)
    Native.Runtime_sleep(1, 20)
    assert(not pcall(Native.Runtime_sleep, 1, 1))
    local ids = Native.Runtime_poll()
    assert(#ids == 1 and ids[1] == 1)
    for _, value in ipairs({-2, 1.5, "0", false, {}, 2147483648}) do
      assert(not pcall(Native.Runtime_poll, value))
    end
    assert(not pcall(Native.Runtime_poll, 0, 0))
  )lua");
}

TEST_F(AsyncTest, AwaitIsLazyAndPreservesMultipleResults) {
  Run(R"lua(
    local started = false
    local child = Task.create(function()
      started = true
      Runtime.sleep(15):await()
      return "done", nil, 7
    end)
    assert(not started and not child:is_done() and not child:is_cancelled())
    assert(not pcall(child.await, child))
    assert(not Scheduler.has_pending())
    local root = spawn(function()
      local result = table.pack(child:await())
      assert(result.n == 3 and result[1] == "done" and result[3] == 7)
      local again = table.pack(child:await())
      assert(again.n == 3 and again[3] == 7)
      return 42
    end)
    drain()
    assert(started and root:is_done() and child:is_done())
    assert(root:await() == 42)
    assert(not Scheduler.run())
  )lua");
}

TEST_F(AsyncTest, ConcurrentTasksShareOneAwaitedTask) {
  Run(R"lua(
    local runs, completed = 0, 0
    local shared = Task.create(function()
      runs = runs + 1
      Runtime.sleep(20):await()
      return 8
    end)
    for _ = 1, 3 do
      spawn(function()
        assert(shared:await() == 8)
        completed = completed + 1
      end)
    end
    drain()
    assert(runs == 1 and completed == 3)
  )lua");
}

TEST_F(AsyncTest, SleepDoesNotBlockOtherReadyTasksOrFinishEarly) {
  const auto start = std::chrono::steady_clock::now();
  Run(R"lua(
    local sequence = {}
    local slow = spawn(function()
      sequence[#sequence + 1] = "start"
      Runtime.sleep(60):await()
      sequence[#sequence + 1] = "end"
    end)
    spawn(function()
      Runtime.sleep(0):await()
      sequence[#sequence + 1] = "other"
    end)
    drain()
    assert(slow:is_done())
    assert(table.concat(sequence, ",") == "start,other,end")
  )lua");
  EXPECT_GE(std::chrono::steady_clock::now() - start,
            std::chrono::milliseconds(60));
}

TEST_F(AsyncTest, AwaitedErrorsAreCatchableAndRootErrorsCleanup) {
  Run(R"lua(
    local child = Task.create(function()
      Runtime.sleep(0):await()
      error("child failure")
    end)
    local root = spawn(function()
      local ok, err = pcall(child.await, child)
      assert(not ok and err:find("child failure", 1, true))
      Runtime.sleep(0):await()
      return "caught"
    end)
    drain()
    assert(root:await() == "caught")
    spawn(function() Runtime.sleep(1000):await() end)
    spawn(function() error("root failure") end)
    local ok, err = pcall(Scheduler.run)
    assert(not ok and err:find("root failure", 1, true))
    assert(not Scheduler.has_pending())
    assert(not pcall(Native.Runtime_poll, 0))
    Scheduler.initialize()
    spawn(function() Runtime.sleep(0):await() end)
    drain()
  )lua");
}

TEST_F(AsyncTest, FailedTasksAndShutdownCloseCoroutineResources) {
  Run(R"lua(
    local closed = 0
    local function resource()
      return setmetatable({}, { __close = function() closed = closed + 1 end })
    end
    local failed = Task.create(function()
      local guard <close> = resource()
      error("guard failure")
    end)
    spawn(function()
      local ok, err = pcall(failed.await, failed)
      assert(not ok and err:find("guard failure", 1, true))
      assert(closed == 1)
    end)
    drain()
    spawn(function()
      local guard <close> = resource()
      Runtime.sleep(1000):await()
    end)
    assert(Scheduler.run())
    assert(Scheduler.run())
    Scheduler.shutdown()
    assert(closed == 2)
    assert(not Scheduler.has_pending())
    Scheduler.shutdown()
  )lua");
}

TEST_F(AsyncTest,
       PendingSleepCannotBeSubmittedEarlyAndNativeErrorsAreAwaitable) {
  Run(R"lua(
    local sleep = Runtime.sleep(20)
    Scheduler.prepare(sleep)
    Scheduler.submit(sleep:get_id())
    assert(Scheduler.run())
    local ok, err = pcall(Scheduler.submit, sleep:get_id())
    assert(not ok and err:find("before its I/O completes", 1, true))
    drain()
    local original = Native.Runtime_sleep
    Native.Runtime_sleep = function() error("timer registration failure") end
    spawn(function()
      local task = Runtime.sleep(0)
      local success, message = pcall(task.await, task)
      assert(not success and message:find("timer registration failure", 1, true))
    end)
    drain()
    Native.Runtime_sleep = original
    spawn(function()
      local bad = Task.create(function() coroutine.yield("unsupported") end)
      local success, message = pcall(bad.await, bad)
      assert(not success and message:find("tasks must yield", 1, true))
      assert(not pcall(Scheduler.run))
      assert(not pcall(Scheduler.submit, Scheduler.get_running():get_id()))
    end)
    drain()
  )lua");
}

TEST_F(AsyncTest, RejectsCyclesInvalidYieldsAndInvalidSleep) {
  Run(R"lua(
    for _, value in ipairs({-1, 1.5, "1", false, {}, 2147483648}) do
      assert(not pcall(Runtime.sleep, value))
    end
    assert(not pcall(Runtime.sleep))
    local a, b
    a = Task.create(function() b:await() end)
    b = Task.create(function() a:await() end)
    Scheduler.prepare(a)
    Scheduler.submit(a:get_id())
    local ok, err = pcall(drain)
    assert(not ok and err:find("cyclic task await", 1, true))
    assert(not Scheduler.has_pending())
    Scheduler.initialize()
    spawn(function() coroutine.yield("invalid") end)
    ok, err = pcall(drain)
    assert(not ok and err:find("tasks must yield", 1, true))
    Scheduler.initialize()
    local suspended = Task.create(function() end)
    Scheduler.prepare(suspended)
    ok, err = pcall(Scheduler.run)
    assert(not ok and err:find("scheduler stalled", 1, true))
  )lua");
}

TEST_F(AsyncTest, PreparedTaskStartsWhenAwaitedAndSubmissionIsIdempotent) {
  Run(R"lua(
    local child = Task.create(function() return "prepared" end)
    Scheduler.prepare(child)
    local root = spawn(function() return child:await() end)
    Scheduler.submit(root:get_id())
    drain()
    assert(root:await() == "prepared")
    assert(not pcall(Scheduler.submit, 999999))
    assert(not pcall(Scheduler.prepare, root))
    assert(not pcall(Task.create, "invalid"))
  )lua");
}

TEST_F(AsyncTest, NewVmDiscardsPreviousPendingTimersAndRejectsOldVm) {
  Run("Native.Runtime_sleep(123, 1000)");
  auto* previous_state = state_;
  state_ = luaL_newstate();
  ASSERT_NE(state_, nullptr);
  luaL_openlibs(state_);
  InstallNativeStorage(state_);
  ConfigureNativePath(state_);
  Run(R"lua(
    PressKey = function() end
    ReleaseKey = function() end
    PressAndReleaseKey = function() end
    IsKeyLockOn = function() return false end
    PressMouseButton = function() end
    ReleaseMouseButton = function() end
    PressAndReleaseMouseButton = function() end
    GetMousePosition = function() return 0, 0 end
    MoveMouseToVirtual = function() end
    MoveMouseRelative = function() end
    MoveMouseWheel = function() end
    Native = require("executor.core.Native")
    assert(#Native.Runtime_poll(0) == 0)
    Native.Runtime_sleep(456, 0)
    local ids = Native.Runtime_poll()
    assert(#ids == 1 and ids[1] == 456)
  )lua");
  EXPECT_EQ(luaL_dostring(previous_state,
                          R"lua(
                            assert(not pcall(Native.Runtime_poll, 0))
                            assert(not pcall(Native.Runtime_open))
                            assert(not pcall(Native.Runtime_close))
                            assert(not pcall(Native.Runtime_sleep, 999, 0))
                          )lua"),
            LUA_OK);
  lua_close(previous_state);
  Run("assert(#Native.Runtime_poll(0) == 0)");
}

TEST_F(AsyncTest,
       NativeModuleLoadsThroughLuaAndFinalizesCallbacksBeforeUnload) {
  Run("Native.Runtime_sleep(987, 1000)");
  auto* storage = &GetStorage(state_);
  lua_close(state_);
  state_ = nullptr;
  EXPECT_EQ(GetModuleHandleA("Native.dll"), nullptr);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  state_ = luaL_newstate();
  ASSERT_NE(state_, nullptr);
  luaL_openlibs(state_);
  InstallNativeStorage(state_);
  ConfigureNativePath(state_);
  Run(R"lua(
    PressKey = function() end
    ReleaseKey = function() end
    PressAndReleaseKey = function() end
    IsKeyLockOn = function() return false end
    PressMouseButton = function() end
    ReleaseMouseButton = function() end
    PressAndReleaseMouseButton = function() end
    GetMousePosition = function() return 0, 0 end
    MoveMouseToVirtual = function() end
    MoveMouseRelative = function() end
    MoveMouseWheel = function() end
  )lua");
  Run("Native = require('executor.core.Native')");
  EXPECT_EQ(&GetStorage(state_), storage);
  EXPECT_NE(GetStorage(state_).runtime, nullptr);
}
