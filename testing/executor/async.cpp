#include <Windows.h>
#include <gtest/gtest.h>
#include <lghubex/dll/hook.h>

#include <chrono>
#include <executor/native/global_storage.hpp>
#include <executor/native/runtime.hpp>
#include <filesystem>
#include <thread>

class AsyncTest : public ::testing::Test {
 protected:
  void SetUp() override {
    state_ = luaL_newstate();
    ASSERT_NE(state_, nullptr);
    luaL_openlibs(state_);
    InstallNativeStorage(state_);
    luaL_getsubtable(state_, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
    lua_pushcfunction(state_, luaopen_executor_Native);
    lua_setfield(state_, -2, "executor.Native");
    const std::filesystem::path root(TEST_PROJECT_ROOT);
    for (const auto* name : {"Task", "Scheduler", "Runtime"}) {
      const auto path =
          root / "executor" / "Core" / (std::string(name) + ".lua");
      ASSERT_EQ(luaL_loadfile(state_, path.string().c_str()), LUA_OK)
          << lua_tostring(state_, -1);
      const auto module = std::string("executor.Core.") + name;
      lua_setfield(state_, -2, module.c_str());
    }
    lua_pop(state_, 1);
    Run(R"lua(
      PressKey = function() end
      ReleaseKey = function() end
      PressMouseButton = function() end
      ReleaseMouseButton = function() end
      Native = require("executor.Native")
      Task = require("executor.Core.Task")
      Scheduler = require("executor.Core.Scheduler")
      Runtime = require("executor.Core.Runtime")
      function spawn(fn)
        local task = Task.Create(fn)
        Scheduler.Prepare(task)
        Scheduler.Submit(task:GetId())
        return task
      end
      function drain()
        while Scheduler.Run() do end
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
    local child = Task.Create(function()
      started = true
      Runtime.Sleep(15):Await()
      return "done", nil, 7
    end)
    assert(not started and not child:IsDone() and not child:IsCancelled())
    assert(not pcall(child.Await, child))
    assert(not Scheduler.HasPending())
    local root = spawn(function()
      local result = table.pack(child:Await())
      assert(result.n == 3 and result[1] == "done" and result[3] == 7)
      local again = table.pack(child:Await())
      assert(again.n == 3 and again[3] == 7)
      return 42
    end)
    drain()
    assert(started and root:IsDone() and child:IsDone())
    assert(root:Await() == 42)
    assert(not Scheduler.Run())
  )lua");
}

TEST_F(AsyncTest, ConcurrentTasksShareOneAwaitedTask) {
  Run(R"lua(
    local runs, completed = 0, 0
    local shared = Task.Create(function()
      runs = runs + 1
      Runtime.Sleep(20):Await()
      return 8
    end)
    for _ = 1, 3 do
      spawn(function()
        assert(shared:Await() == 8)
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
      Runtime.Sleep(60):Await()
      sequence[#sequence + 1] = "end"
    end)
    spawn(function()
      Runtime.Sleep(0):Await()
      sequence[#sequence + 1] = "other"
    end)
    drain()
    assert(slow:IsDone())
    assert(table.concat(sequence, ",") == "start,other,end")
  )lua");
  EXPECT_GE(std::chrono::steady_clock::now() - start,
            std::chrono::milliseconds(60));
}

TEST_F(AsyncTest, AwaitedErrorsAreCatchableAndRootErrorsCleanup) {
  Run(R"lua(
    local child = Task.Create(function()
      Runtime.Sleep(0):Await()
      error("child failure")
    end)
    local root = spawn(function()
      local ok, err = pcall(child.Await, child)
      assert(not ok and err:find("child failure", 1, true))
      Runtime.Sleep(0):Await()
      return "caught"
    end)
    drain()
    assert(root:Await() == "caught")
    spawn(function() Runtime.Sleep(1000):Await() end)
    spawn(function() error("root failure") end)
    local ok, err = pcall(Scheduler.Run)
    assert(not ok and err:find("root failure", 1, true))
    assert(not Scheduler.HasPending())
    assert(not pcall(Native.Runtime_poll, 0))
    Scheduler.Initialize()
    spawn(function() Runtime.Sleep(0):Await() end)
    drain()
  )lua");
}

TEST_F(AsyncTest, FailedTasksAndShutdownCloseCoroutineResources) {
  Run(R"lua(
    local closed = 0
    local function resource()
      return setmetatable({}, { __close = function() closed = closed + 1 end })
    end
    local failed = Task.Create(function()
      local guard <close> = resource()
      error("guard failure")
    end)
    spawn(function()
      local ok, err = pcall(failed.Await, failed)
      assert(not ok and err:find("guard failure", 1, true))
      assert(closed == 1)
    end)
    drain()
    spawn(function()
      local guard <close> = resource()
      Runtime.Sleep(1000):Await()
    end)
    assert(Scheduler.Run())
    assert(Scheduler.Run())
    Scheduler.Shutdown()
    assert(closed == 2)
    assert(not Scheduler.HasPending())
    Scheduler.Shutdown()
  )lua");
}

TEST_F(AsyncTest,
       PendingSleepCannotBeSubmittedEarlyAndNativeErrorsAreAwaitable) {
  Run(R"lua(
    local sleep = Runtime.Sleep(20)
    Scheduler.Prepare(sleep)
    Scheduler.Submit(sleep:GetId())
    assert(Scheduler.Run())
    local ok, err = pcall(Scheduler.Submit, sleep:GetId())
    assert(not ok and err:find("before its I/O completes", 1, true))
    drain()
    local original = Native.Runtime_sleep
    Native.Runtime_sleep = function() error("timer registration failure") end
    spawn(function()
      local task = Runtime.Sleep(0)
      local success, message = pcall(task.Await, task)
      assert(not success and message:find("timer registration failure", 1, true))
    end)
    drain()
    Native.Runtime_sleep = original
    spawn(function()
      local bad = Task.Create(function() coroutine.yield("unsupported") end)
      local success, message = pcall(bad.Await, bad)
      assert(not success and message:find("tasks must yield", 1, true))
      assert(not pcall(Scheduler.Run))
      assert(not pcall(Scheduler.Submit, Scheduler.GetCurrent():GetId()))
    end)
    drain()
  )lua");
}

TEST_F(AsyncTest, RejectsCyclesInvalidYieldsAndInvalidSleep) {
  Run(R"lua(
    for _, value in ipairs({-1, 1.5, "1", false, {}, 2147483648}) do
      assert(not pcall(Runtime.Sleep, value))
    end
    assert(not pcall(Runtime.Sleep))
    local a, b
    a = Task.Create(function() b:Await() end)
    b = Task.Create(function() a:Await() end)
    Scheduler.Prepare(a)
    Scheduler.Submit(a:GetId())
    local ok, err = pcall(drain)
    assert(not ok and err:find("cyclic task await", 1, true))
    assert(not Scheduler.HasPending())
    Scheduler.Initialize()
    spawn(function() coroutine.yield("invalid") end)
    ok, err = pcall(drain)
    assert(not ok and err:find("tasks must yield", 1, true))
    Scheduler.Initialize()
    local suspended = Task.Create(function() end)
    Scheduler.Prepare(suspended)
    ok, err = pcall(Scheduler.Run)
    assert(not ok and err:find("scheduler stalled", 1, true))
  )lua");
}

TEST_F(AsyncTest, PreparedTaskStartsWhenAwaitedAndSubmissionIsIdempotent) {
  Run(R"lua(
    local child = Task.Create(function() return "prepared" end)
    Scheduler.Prepare(child)
    local root = spawn(function() return child:Await() end)
    Scheduler.Submit(root:GetId())
    drain()
    assert(root:Await() == "prepared")
    assert(not pcall(Scheduler.Submit, 999999))
    assert(not pcall(Scheduler.Prepare, root))
    assert(not pcall(Task.Create, "invalid"))
  )lua");
}

TEST_F(AsyncTest, NewVmDiscardsPreviousPendingTimersAndRejectsOldVm) {
  Run("Native.Runtime_sleep(123, 1000)");
  auto* previous_state = state_;
  state_ = luaL_newstate();
  ASSERT_NE(state_, nullptr);
  luaL_openlibs(state_);
  InstallNativeStorage(state_);
  lua_pushcfunction(state_, luaopen_executor_Native);
  lua_setglobal(state_, "openNative");
  Run(R"lua(
    PressKey = function() end
    ReleaseKey = function() end
    PressMouseButton = function() end
    ReleaseMouseButton = function() end
    Native = openNative()
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

TEST_F(AsyncTest, HookCallbacksSurviveActualNativeDllUnload) {
  HMODULE module = LoadLibraryA(NATIVE_DLL_PATH);
  ASSERT_NE(module, nullptr);
  auto open = reinterpret_cast<lua_CFunction>(
      GetProcAddress(module, "luaopen_executor_Native"));
  ASSERT_NE(open, nullptr);
  lua_pushcfunction(state_, open);
  lua_setglobal(state_, "openDllNative");
  Run("openDllNative().Runtime_sleep(987, 10)");
  lua_close(state_);
  state_ = nullptr;
  ASSERT_TRUE(FreeLibrary(module));
  EXPECT_EQ(GetModuleHandleA(NATIVE_DLL_PATH), nullptr);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  state_ = luaL_newstate();
  ASSERT_NE(state_, nullptr);
  luaL_openlibs(state_);
  InstallNativeStorage(state_);
  EXPECT_EQ(GetStorage(state_).runtime, nullptr);
}
