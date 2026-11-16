#include <Windows.h>
#include <gtest/gtest.h>
#include <lghubex/dll/hook.h>

#include <chrono>
#include <executor/core/Native/global_storage.hpp>
#include <executor/core/Native/input.hpp>
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

class NativeInputTest : public ::testing::Test {
 protected:
  void SetUp() override {
    state = luaL_newstate();
    ASSERT_NE(state, nullptr);
    luaL_openlibs(state);
    InstallNativeStorage(state);
    ConfigureNativePath(state);
    InitializeStorage(state);
    GetStorage(state).keyboard.Reset(state);
    GetStorage(state).mouse.Reset(state);
    Run(R"lua(
            calls = {}
            function asSet(values)
                local result = {}
                for _, value in ipairs(values) do result[value] = true end
                return result
            end
            local function record(name, ...)
                calls[#calls + 1] = {name, ...}
            end
            function PressKey(...) record("pressKey", ...) end
            function ReleaseKey(...) record("releaseKey", ...) end
            function PressMouseButton(...) record("pressButton", ...) end
            function ReleaseMouseButton(...) record("releaseButton", ...) end
            function Sleep(...) record("sleep", ...) end
            function OutputLogMessage(...) record("log", ...) end
            function PressAndReleaseKey(...) record("clickKey", ...) end
            function GetMousePosition() return 12, 34 end
            rawSleep = Sleep
            rawLog = OutputLogMessage
            rawClick = PressAndReleaseKey
            rawPosition = GetMousePosition
            rawPress = PressKey
            rawRelease = ReleaseKey
            rawPressButton = PressMouseButton
            rawReleaseButton = ReleaseMouseButton
            builtinNames = {
                "PressAndReleaseMouseButton", "PressAndReleaseHidKey", "PressAndReleaseKey",
                "PlayMacro", "EnablePrimaryMouseButtonEvents", "SetMouseDPITableIndex",
                "SetMouseDPITable", "GetMouseSpeed", "ReleaseHidKey", "DecrementMouseSpeed",
                "IncrementMouseSpeed", "SetBacklightColor", "PressMouseButton", "ReleaseKey",
                "SetMKeyState", "IsModifierPressed", "GetMKeyState", "AbortMacro", "ReleaseMacro",
                "GetRunningTime", "GetDate", "Raw_sleep", "MoveMouseTo", "OutputDebugMessage",
                "PressMacro", "GetMousePosition", "PressHidKey", "MoveMouseToVirtual",
                "MoveMouseRelative", "MoveMouseWheel", "IsMouseButtonPressed", "SetMouseSpeed",
                "ClearLog", "OutputLogMessage", "IsKeyLockOn", "ReleaseMouseButton", "PressKey"
            }
            for _, name in ipairs(builtinNames) do
                if _G[name] == nil then
                    _G[name] = function(...) record(name, ...) end
                end
            end
        )lua");
    Run("calls = {}");
  }

  void TearDown() override {
    if (state != nullptr) {
      lua_close(state);
    }
  }

  void Run(const char* script) {
    const int status = luaL_dostring(state, script);
    ASSERT_EQ(status, LUA_OK)
        << (lua_tostring(state, -1) != nullptr ? lua_tostring(state, -1) : "");
  }

  lua_Integer MonotonicTimepoint() {
    GetStorage(state).time.GetMonotonicTimestamp(state);
    const lua_Integer timepoint = lua_tointeger(state, -1);
    lua_pop(state, 1);
    return timepoint;
  }

  lua_State* state = nullptr;
};

TEST_F(NativeInputTest, FreshRequireDoesNotReleaseUntrackedInputs) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        assert(#calls == 0)
        local count = #calls
        assert(Native == require("executor.core.Native") and #calls == count)
        assert(#Native.Keyboard_list_pressed() == 0)
        assert(#Native.Mouse_list_pressed() == 0)
        calls = {}
        Native.Keyboard_reset()
        Native.Mouse_reset()
        assert(#calls == 0)
    )lua");
}

TEST_F(NativeInputTest, PreservesLegacyGlobalsAndExportsOnlyMinimalApi) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        assert(Native == package.loaded["executor.core.Native"])
        assert(Native == require("executor.core.Native"))
        assert(Native.Raw_sleep ~= rawSleep)
        assert(Native.Keyboard_click ~= rawClick)
        assert(Native.Mouse_get_position ~= rawPosition)
        assert(Native.Keyboard_press ~= rawPress)
        assert(Native.Keyboard_release ~= rawRelease)
        assert(Native.Mouse_press ~= rawPressButton)
        assert(Native.Mouse_release ~= rawReleaseButton)
        assert(PressKey == rawPress and ReleaseKey == rawRelease)
        assert(PressMouseButton == rawPressButton and ReleaseMouseButton == rawReleaseButton)
        assert(Sleep == rawSleep and OutputLogMessage == rawLog)
        assert(PressAndReleaseKey == rawClick and GetMousePosition == rawPosition)
        assert(type(require) == "function" and type(pairs) == "function")
        local expected = asSet({
            "Keyboard_press", "Keyboard_release", "Keyboard_click", "Keyboard_reset",
            "Keyboard_list_pressed", "Keyboard_is_pressed", "Keyboard_is_locked",
            "Mouse_press", "Mouse_release", "Mouse_click", "Mouse_reset",
            "Mouse_list_pressed", "Mouse_is_pressed", "Mouse_get_position",
            "Mouse_set_position", "Mouse_move", "Mouse_scroll",
            "Time_get_monotonic_timestamp", "Time_get_system_timestamp", "Raw_sleep",
            "Raw_get_thread_id", "Runtime_open", "Runtime_sleep",
            "Runtime_poll", "Runtime_close"
        })
        for name, value in pairs(Native) do
            assert(expected[name] and type(value) == "function", name)
            expected[name] = nil
        end
        assert(next(expected) == nil)
        assert(type(Native.Time_get_monotonic_timestamp) == "function")
        assert(type(Native.Time_get_system_timestamp) == "function")
        for _, name in ipairs(builtinNames) do
            assert(Native[name] == nil or name == "Raw_sleep", name)
            assert(type(_G[name]) == "function", name)
        end
        local x, y = Native.Mouse_get_position()
        assert(x == 12 and y == 34)
        calls = {}
        Native.Raw_sleep(1)
        assert(#calls == 0)
    )lua");
}

TEST_F(NativeInputTest, ForwardsMixedVariadicKeysAndReleasesAllTrackedKeys) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        calls = {}
        Native.Keyboard_press(30, "b", "lctrl")
        assert(#calls == 1)
        assert(calls[1][2] == 30 and calls[1][3] == "b" and calls[1][4] == "lctrl")
        local pressed = Native.Keyboard_list_pressed()
        local held = asSet(pressed)
        assert(#pressed == 3 and held.lctrl and held.b and held[30])
        pressed[1] = "not a key"
        Native.Keyboard_reset()
        assert(#calls == 4)
        local released = {}
        for i = 2, 4 do
            assert(calls[i][1] == "releaseKey" and not released[calls[i][2]])
            released[calls[i][2]] = true
        end
        assert(released.lctrl and released.b and released[30])
        assert(#Native.Keyboard_list_pressed() == 0)
        Native.Keyboard_reset()
        assert(#calls == 4)
    )lua");
}

TEST_F(NativeInputTest, RejectsInvalidSequencesBeforeAnyHostCall) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        Native.Keyboard_press("a")
        calls = {}
        local invalid = {"?", "", "A", "ctrl", "shift", "alt", "30",
                         "a\0bad", 0, -1, 0x10000, 30.5, true, {}, function() end}
        for _, value in ipairs(invalid) do
            assert(not pcall(Native.Keyboard_press, 30, value))
            assert(not pcall(Native.Keyboard_release, 30, value))
        end
        assert(not pcall(Native.Keyboard_press, 30, nil))
        assert(not pcall(Native.Keyboard_release, 30, nil))
        assert(not pcall(Native.Keyboard_press))
        assert(not pcall(Native.Keyboard_release))
        assert(#calls == 0)
        local pressed = Native.Keyboard_list_pressed()
        assert(#pressed == 1 and pressed[1] == "a")
    )lua");
}

TEST_F(NativeInputTest, TreatsNamesAndScanCodesAsOnePressedKey) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        Native.Keyboard_press("a", "b", "c")
        Native.Keyboard_press(30)
        assert(#Native.Keyboard_list_pressed() == 3)
        Native.Keyboard_release(48)
        local pressed = Native.Keyboard_list_pressed()
        local held = asSet(pressed)
        assert(#pressed == 2 and held.c and held.a)
        Native.Keyboard_release(30)
        Native.Keyboard_press(48)
        Native.Keyboard_release("b")
        calls = {}
        Native.Keyboard_reset()
        assert(#calls == 1 and calls[1][2] == "c")
        Native.Keyboard_press(30.0)
        Native.Keyboard_release("a")
        assert(#Native.Keyboard_list_pressed() == 0)
    )lua");
}

TEST_F(NativeInputTest, HandlesLargeVariadicSequences) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        local keys = {}
        for i = 1, 1000 do keys[i] = i % 2 == 0 and 30 or "b" end
        calls = {}
        Native.Keyboard_press(table.unpack(keys))
        assert(#calls == 1 and #calls[1] == 1001)
        assert(#Native.Keyboard_list_pressed() == 2)
        Native.Keyboard_release(table.unpack(keys))
        assert(#calls == 2 and #calls[2] == 1001)
        assert(#Native.Keyboard_list_pressed() == 0)
        keys[1000] = "?"
        assert(not pcall(Native.Keyboard_press, table.unpack(keys)))
        assert(#calls == 2)
    )lua");
}

TEST_F(NativeInputTest, AllowsAbsentOptionalHostFunctions) {
  Run(R"lua(
        GetDate = nil
        local Native = require("executor.core.Native")
        assert(Native.GetDate == nil and GetDate == nil)
        Native.Keyboard_press("a")
        assert(Native.Keyboard_list_pressed()[1] == "a")
    )lua");
}

TEST_F(NativeInputTest, TracksMouseButtonsSeparatelyWithoutDuplicates) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        Native.Mouse_press(1)
        Native.Keyboard_press("a")
        Native.Mouse_press(3)
        Native.Mouse_press(5)
        Native.Mouse_press(1)
        Native.Mouse_release(3)
        local buttons = Native.Mouse_list_pressed()
        local held = asSet(buttons)
        assert(#buttons == 2 and held[5] and held[1])
        calls = {}
        Native.Mouse_reset()
        assert(#calls == 2)
        local released = asSet({calls[1][2], calls[2][2]})
        assert(released[5] and released[1])
        assert(#Native.Mouse_list_pressed() == 0)
        assert(#Native.Keyboard_list_pressed() == 1)
        Native.Keyboard_reset()
        assert(calls[3][1] == "releaseKey" and calls[3][2] == "a")
    )lua");
}

TEST_F(NativeInputTest, RejectsInvalidMouseArgumentsWithoutChangingState) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        Native.Mouse_press(1)
        calls = {}
        for _, value in ipairs({0, 6, -1, 1.5, "1", false, {}}) do
            assert(not pcall(Native.Mouse_press, value))
            assert(not pcall(Native.Mouse_release, value))
        end
        assert(not pcall(Native.Mouse_press))
        assert(not pcall(Native.Mouse_release))
        assert(not pcall(Native.Mouse_press, 1, 6))
        assert(not pcall(Native.Mouse_release, 1, 6))
        assert(#calls == 0)
        assert(Native.Mouse_list_pressed()[1] == 1)
    )lua");
}

TEST_F(NativeInputTest, PreservesHostErrorsAndRetainsUnreleasedEntries) {
  Run(R"lua(
        ReleaseKey = function(key)
            if key == "b" then error("host release failed") end
            calls[#calls + 1] = {"releaseKey", key}
        end
        local Native = require("executor.core.Native")
        Native.Keyboard_press("a", "b", "c")
        calls = {}
        local ok, message = pcall(Native.Keyboard_reset)
        assert(not ok and message:find("host release failed", 1, true))
        local pressed = Native.Keyboard_list_pressed()
        local held = asSet(pressed)
        assert(held.b)
        assert(#pressed + #calls == 3)
        for _, call in ipairs(calls) do assert(not held[call[2]]) end
    )lua");
}

TEST_F(NativeInputTest, PreservesRawReturnValuesAndDoesNotTrackFailedPresses) {
  Run(R"lua(
        PressKey = function(key)
            if key == "b" then error("host press failed") end
            return "result", nil, 42
        end
        local Native = require("executor.core.Native")
        local result = table.pack(Native.Keyboard_press("a"))
        assert(result.n == 3 and result[1] == "result" and result[2] == nil and result[3] == 42)
        local ok, message = pcall(Native.Keyboard_press, "b")
        assert(not ok and message:find("host press failed", 1, true))
        local pressed = Native.Keyboard_list_pressed()
        assert(#pressed == 1 and pressed[1] == "a")
    )lua");
}

TEST_F(NativeInputTest,
       MissingHostFunctionsFailExplicitlyWithoutMovingGlobals) {
  Run(R"lua(
        PressKey = nil
        local ok, message = pcall(require, "executor.core.Native")
        assert(not ok and message:find("Logitech function PressKey is unavailable", 1, true))
        assert(ReleaseKey == rawRelease and Sleep == rawSleep)
        assert(package.loaded["executor.core.Native"] == nil)
    )lua");
}

TEST_F(NativeInputTest, ForwardsPauseWithoutSpecialHandling) {
  Run(R"lua(
        local function host(name, ...)
            for _, key in ipairs({...}) do
                assert(type(key) == "number" or type(key) == "string")
            end
            calls[#calls + 1] = {name, ...}
        end
        PressKey = function(...) host("pressKey", ...) end
        ReleaseKey = function(...) host("releaseKey", ...) end
        local Native = require("executor.core.Native")
        calls = {}
        Native.Keyboard_press(30, 0x146)
        assert(calls[1][2] == 30 and calls[1][3] == 0x146)
        assert(asSet(Native.Keyboard_list_pressed())[0x146])
        Native.Keyboard_release(0x146)
        assert(calls[2][2] == 0x146)
        Native.Keyboard_press("pause")
        calls = {}
        Native.Keyboard_reset()
        local released = asSet({calls[1][2], calls[2][2]})
        assert(#calls == 2 and released.pause and released[30])
        assert(#Native.Keyboard_list_pressed() == 0)
        calls = {}
        assert(not pcall(Native.Keyboard_press, 0x146, "?"))
        assert(#calls == 0)
    )lua");
}

TEST_F(NativeInputTest, IdentifiesTheKeyRejectedByTheHostDuringInitialization) {
  lua_pushinteger(state, 0x146);
  GetStorage(state).keyboard.Press(state);
  lua_settop(state, 0);
  Run("calls = {}");
  Run(R"lua(
        ReleaseKey = function(key)
            if key == 0x146 then error("invalid argument") end
        end
        local ok, message = pcall(require, "executor.core.Native")
        assert(not ok)
        assert(message:find("ReleaseKey for tracked input 326 failed:", 1, true), message)
        assert(message:find("invalid argument", 1, true), message)
        assert(Sleep == rawSleep and PressKey == rawPress)
        assert(package.loaded["executor.core.Native"] == nil)
        ReleaseKey = rawRelease
        local Native = require("executor.core.Native")
        assert(#calls == 1 and calls[1][1] == "releaseKey" and calls[1][2] == 0x146)
        assert(#Native.Keyboard_list_pressed() == 0)
    )lua");
}

TEST_F(NativeInputTest,
       IdentifiesTheButtonRejectedByTheHostDuringInitialization) {
  lua_pushinteger(state, 4);
  GetStorage(state).mouse.Press(state);
  lua_settop(state, 0);
  Run("calls = {}");
  Run(R"lua(
        ReleaseMouseButton = function(button)
            if button == 4 then error("invalid argument") end
        end
        local ok, message = pcall(require, "executor.core.Native")
        assert(not ok)
        assert(message:find("ReleaseMouseButton for tracked input 4 failed:", 1, true), message)
        assert(message:find("invalid argument", 1, true), message)
        assert(Sleep == rawSleep and PressKey == rawPress)
        assert(package.loaded["executor.core.Native"] == nil)
        ReleaseMouseButton = rawReleaseButton
        local Native = require("executor.core.Native")
        assert(#calls == 1 and calls[1][1] == "releaseButton" and calls[1][2] == 4)
        assert(#Native.Mouse_list_pressed() == 0)
    )lua");
}

TEST_F(NativeInputTest, NewVmReleasesPressesLeftByDestroyedVm) {
  Run(R"lua(
        Native = require("executor.core.Native")
        Native.Keyboard_press("a")
        Native.Keyboard_press(48)
        Native.Keyboard_press("c")
        Native.Keyboard_release("c")
        Native.Mouse_press(1)
        Native.Mouse_press(3)
        Native.Mouse_release(3)
    )lua");
  auto* storage = &GetStorage(state);
  lua_close(state);
  state = nullptr;
  lua_State* other = luaL_newstate();
  ASSERT_NE(other, nullptr);
  luaL_openlibs(other);
  InstallNativeStorage(other);
  ConfigureNativePath(other);
  ASSERT_EQ(&GetStorage(other), storage);
  const int status = luaL_dostring(other, R"lua(
        PressKey = function() end
        local keys, buttons = {}, {}
        ReleaseKey = function(key) keys[key] = (keys[key] or 0) + 1 end
        PressAndReleaseKey = function() end
        IsKeyLockOn = function() return false end
        PressMouseButton = function() end
        ReleaseMouseButton = function(button) buttons[button] = (buttons[button] or 0) + 1 end
        PressAndReleaseMouseButton = function() end
        GetMousePosition = function() return 0, 0 end
        MoveMouseToVirtual = function() end
        MoveMouseRelative = function() end
        MoveMouseWheel = function() end
        local Native = require("executor.core.Native")
        assert(keys.a == 1 and keys[48] == 1 and keys.c == nil)
        assert(buttons[1] == 1 and buttons[3] == nil)
        assert(keys[0x146] == nil and keys.pause == nil)
        assert(#Native.Keyboard_list_pressed() == 0 and #Native.Mouse_list_pressed() == 0)
        Native.Keyboard_press("b")
        Native.Mouse_press(2)
        Native.Keyboard_reset()
        Native.Mouse_reset()
    )lua");
  const std::string error = status == LUA_OK ? "" : lua_tostring(other, -1);
  lua_close(other);
  ASSERT_EQ(status, LUA_OK) << error;
}

TEST_F(NativeInputTest, TimerOriginSurvivesVmRecreation) {
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  const auto before = MonotonicTimepoint();
  ASSERT_GE(before, 20);
  EXPECT_GE(MonotonicTimepoint(), before);
  lua_close(state);
  state = luaL_newstate();
  ASSERT_NE(state, nullptr);
  luaL_openlibs(state);
  InstallNativeStorage(state);
  EXPECT_GE(MonotonicTimepoint(), before);
}

TEST_F(NativeInputTest, TimeApisReturnIntegerMilliseconds) {
  lua_newtable(state);
  InitializeTime(state);
  lua_setglobal(state, "Native");
  const auto before = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
  Run(R"lua(
        local monotonic = Native.Time_get_monotonic_timestamp()
        assert(math.type(monotonic) == "integer" and monotonic >= 0)
        assert(Native.Time_get_monotonic_timestamp() >= monotonic)
        systemTimepoint = Native.Time_get_system_timestamp()
        assert(math.type(systemTimepoint) == "integer")
    )lua");
  const auto after = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();
  lua_getglobal(state, "systemTimepoint");
  const auto system_timepoint = lua_tointeger(state, -1);
  lua_pop(state, 1);
  EXPECT_GE(system_timepoint, before);
  EXPECT_LE(system_timepoint, after);
}

TEST_F(NativeInputTest, QueriesKeyStateWithoutUsingLegacyOrTrackedState) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        IsModifierPressed = function() error("must use Win32") end
        IsKeyLockOn = function() error("must use Win32") end
        calls = {}
        for _, key in ipairs({
            "a", 30, "ctrl", "shift", "alt", "lctrl", "rctrl", "lshift",
            "rshift", "lalt", "ralt", "lgui", "rgui", "pause", 0x146,
            "printscreen", "numlock", "capslock", "scrolllock", "num7", "num8",
            "num9", "num4", "num5", "num6", "num1", "num2", "num3", "num0",
            "numperiod", "numslash", "numenter", "home", "end", "insert",
            "delete", "pageup", "pagedown", "up", "down", "left", "right",
            "f1", "f24", "non_us_slash"
        }) do
            assert(type(Native.Keyboard_is_pressed(key)) == "boolean", tostring(key))
        end
        for _, key in ipairs({"capslock", "numlock", "scrolllock", 0x3a, 0x45, 0x46}) do
            assert(type(Native.Keyboard_is_locked(key)) == "boolean")
        end
        for _, key in ipairs({"?", "", "A", "30", "a\0bad", 0, -1, 30.5, true, {}}) do
            assert(not pcall(Native.Keyboard_is_pressed, key))
            assert(not pcall(Native.Keyboard_is_locked, key))
        end
        for _, key in ipairs({"a", 30, "ctrl", "shift", "alt"}) do
            assert(not pcall(Native.Keyboard_is_locked, key))
        end
        assert(not pcall(Native.Keyboard_is_pressed))
        assert(not pcall(Native.Keyboard_is_pressed, nil))
        assert(not pcall(Native.Keyboard_is_pressed, "a", "b"))
        assert(not pcall(Native.Keyboard_is_locked))
        assert(not pcall(Native.Keyboard_is_locked, nil))
        assert(not pcall(Native.Keyboard_is_locked, "capslock", "numlock"))
        assert(#calls == 0)
        assert(#Native.Keyboard_list_pressed() == 0)
    )lua");
}

TEST_F(NativeInputTest, ForwardsOnlyReservedMouseApisAtRuntime) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        local calls = {}
        GetMousePosition = function() return 55, 66 end
        MoveMouseTo = function() error("must use virtual desktop") end
        MoveMouseToVirtual = function(...)
            calls[#calls + 1] = {"set", ...}
            return "moved", nil, 7
        end
        MoveMouseRelative = function(...) calls[#calls + 1] = {"move", ...} end
        MoveMouseWheel = function(...) calls[#calls + 1] = {"scroll", ...} end
        IsMouseButtonPressed = function(button)
            calls[#calls + 1] = {"pressed", button}
            return button == 3
        end
        local x, y = Native.Mouse_get_position()
        assert(x == 55 and y == 66)
        local result = table.pack(Native.Mouse_set_position(123, 456))
        assert(result.n == 3 and result[1] == "moved" and result[3] == 7)
        Native.Mouse_move(-2, 3)
        Native.Mouse_scroll(-1)
        assert(Native.Mouse_is_pressed(3) and not Native.Mouse_is_pressed(1))
        assert(#calls == 5)
        assert(calls[1][1] == "set" and calls[1][2] == 123 and calls[1][3] == 456)
        assert(calls[2][1] == "move" and calls[2][2] == -2 and calls[2][3] == 3)
        assert(calls[3][1] == "scroll" and calls[3][2] == -1)
        for _, button in ipairs({0, 6, "1", false, 1.5, {}}) do
            assert(not pcall(Native.Mouse_is_pressed, button))
        end
        assert(not pcall(Native.Mouse_is_pressed))
        assert(not pcall(Native.Mouse_is_pressed, 1, 2))
        assert(#calls == 5)
        assert(#Native.Mouse_list_pressed() == 0)
        MoveMouseWheel = function() error("scroll failed") end
        local ok, err = pcall(Native.Mouse_scroll, 1)
        assert(not ok and err:find("scroll failed", 1, true))
        MoveMouseToVirtual = nil
        ok, err = pcall(Native.Mouse_set_position, 1, 2)
        assert(not ok and err:find("MoveMouseToVirtual is unavailable", 1, true))
    )lua");
}

TEST_F(NativeInputTest, CallsCurrentLegacyGlobalsAtRuntime) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        local pressed, released, buttons = {}, {}, {}
        PressKey = function(...) pressed = {...}; return "new press" end
        ReleaseKey = function(...) released = {...} end
        PressMouseButton = function(button) buttons[button] = true end
        ReleaseMouseButton = function(button) buttons[button] = nil end
        GetMousePosition = function() return 55, 66 end
        assert(Native.Keyboard_press(30, "b") == "new press")
        assert(pressed[1] == 30 and pressed[2] == "b")
        Native.Keyboard_release("a")
        assert(released[1] == "a")
        Native.Keyboard_reset()
        assert(released[1] == "b")
        Native.Mouse_press(1)
        assert(buttons[1])
        Native.Mouse_reset()
        assert(not buttons[1])
        Native.Raw_sleep(1)
        local x, y = Native.Mouse_get_position()
        assert(x == 55 and y == 66)
        assert(Native.Sleep == nil)
        PressKey = rawPress
        PressKey("c")
        assert(#Native.Keyboard_list_pressed() == 0)
    )lua");
}

TEST_F(NativeInputTest, ValidatesPressAndReleaseAndUpdatesTracking) {
  Run(R"lua(
    local Native = require("executor.core.Native")
    Native.Keyboard_press("a", "b", "c")
    Native.Mouse_press(1)
    calls = {}
    assert(not pcall(Native.Keyboard_click, 30, "?"))
    assert(not pcall(Native.Keyboard_click))
    for _, button in ipairs({0, 6, "1", false, 1.5}) do
      assert(not pcall(Native.Mouse_click, button))
    end
    assert(not pcall(Native.Mouse_click))
    assert(not pcall(Native.Mouse_click, 1, 2))
    assert(#calls == 0)
    assert(#Native.Keyboard_list_pressed() == 3)
    assert(#Native.Mouse_list_pressed() == 1)
    PressAndReleaseKey = function(...)
      calls[#calls + 1] = {"click", ...}
      return "clicked", nil, 7
    end
    local result = table.pack(Native.Keyboard_click(30, "b"))
    assert(result.n == 3 and result[1] == "clicked" and result[3] == 7)
    assert(calls[1][2] == 30 and calls[1][3] == "b")
    assert(#Native.Keyboard_list_pressed() == 1 and Native.Keyboard_list_pressed()[1] == "c")
    PressAndReleaseMouseButton = function(button)
      calls[#calls + 1] = {"clickButton", button}
      return "button"
    end
    assert(Native.Mouse_click(1) == "button")
    assert(calls[2][2] == 1 and #Native.Mouse_list_pressed() == 0)
    assert(Native.Mouse_click(2) == "button")
    PressAndReleaseKey = function() error("click failed") end
    local ok, err = pcall(Native.Keyboard_click, "c")
    assert(not ok and err:find("click failed", 1, true))
    assert(Native.Keyboard_list_pressed()[1] == "c")
    Native.Mouse_press(3)
    PressAndReleaseMouseButton = function() error("button failed") end
    assert(not pcall(Native.Mouse_click, 3))
    assert(Native.Mouse_list_pressed()[1] == 3)
    assert(type(PressAndReleaseKey) == "function")
    assert(type(PressAndReleaseMouseButton) == "function")
  )lua");
}

TEST_F(NativeInputTest, MissingStorageBridgeFailsExplicitly) {
  lua_pushnil(state);
  lua_setfield(state, LUA_REGISTRYINDEX, kNativeStorageRegistryKey);
  Run(R"lua(
        local ok, message = pcall(require, "executor.core.Native")
        assert(not ok and message:find("Native storage interface is unavailable", 1, true))
    )lua");
}

TEST_F(NativeInputTest, StorageIsInitializedOnceAndCannotBeReplaced) {
  const int top = lua_gettop(state);
  auto* previous = &GetStorage(state);
  EXPECT_EQ(lua_gettop(state), top);
  SetStorage(state, previous);
  EXPECT_EQ(&GetStorage(state), previous);
  NativeStorage replacement;
  lua_pushlightuserdata(state, &replacement);
  lua_setglobal(state, "replacementStorage");
  Run(R"lua(
        assert(GetStorage == nil and SetStorage == nil)
        local accessors = debug.getregistry()["executor.core.Native.Storage"]
        assert(type(accessors.GetStorage) == "function")
        assert(type(accessors.SetStorage) == "function")
        assert(not pcall(accessors.SetStorage, nil))
        assert(not pcall(accessors.SetStorage, {}))
        local ok, message = pcall(accessors.SetStorage, replacementStorage)
        assert(not ok and message:find("initialized once", 1, true))
        replacementStorage = nil
    )lua");
  EXPECT_EQ(&GetStorage(state), previous);
  InstallNativeStorage(state);
  EXPECT_EQ(&GetStorage(state), previous);
  EXPECT_EQ(lua_gettop(state), top);
}

TEST_F(NativeInputTest, HookStorageSurvivesActualNativeDllUnloadAndReload) {
  Run(R"lua(
        local Native = require("executor.core.Native")
        Native.Keyboard_press("a", 48)
        Native.Mouse_press(1)
    )lua");
  auto* storage = &GetStorage(state);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  const auto before = MonotonicTimepoint();
  ASSERT_GE(before, 20);
  lua_close(state);
  state = nullptr;
  EXPECT_EQ(GetModuleHandleA("Native.dll"), nullptr);
  state = luaL_newstate();
  ASSERT_NE(state, nullptr);
  luaL_openlibs(state);
  InstallNativeStorage(state);
  ConfigureNativePath(state);
  ASSERT_EQ(&GetStorage(state), storage);
  Run(R"lua(
        local keys, buttons = {}, {}
        PressKey = function() end
        ReleaseKey = function(key) keys[key] = true end
        PressAndReleaseKey = function() end
        IsKeyLockOn = function() return false end
        PressMouseButton = function() end
        ReleaseMouseButton = function(button) buttons[button] = true end
        PressAndReleaseMouseButton = function() end
        GetMousePosition = function() return 0, 0 end
        MoveMouseToVirtual = function() end
        MoveMouseRelative = function() end
        MoveMouseWheel = function() end
        local Native = require("executor.core.Native")
        assert(keys.a and keys[48] and buttons[1])
        assert(#Native.Keyboard_list_pressed() == 0 and #Native.Mouse_list_pressed() == 0)
    )lua");
  EXPECT_GE(MonotonicTimepoint(), before);
  lua_close(state);
  state = nullptr;
  EXPECT_EQ(GetModuleHandleA("Native.dll"), nullptr);
}
