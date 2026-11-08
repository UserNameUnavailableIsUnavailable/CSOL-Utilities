#include <Windows.h>
#include <lghubex/dll/hook.h>
#include <lua.hpp>

#include <cstring>
#include <iterator>
#include <string>

#include <gtest/gtest.h>

class InjectionPathTest : public ::testing::Test {
protected:
    void SetUp() override {
        previousDirectory = g_ModuleDirectory;
        state = luaL_newstate();
        ASSERT_NE(state, nullptr);
        luaL_openlibs(state);

        const wchar_t path[] = L"C:\\test\\lghubex";
        directory = static_cast<DllDirectory*>(
            VirtualAlloc(nullptr, sizeof(DllDirectory) + sizeof(path),
                         MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        ASSERT_NE(directory, nullptr);
        std::memcpy(directory->path, path, sizeof(path));
        directory->length = std::size(path) - 1;
        g_ModuleDirectory = directory;
    }

    void TearDown() override {
        g_ModuleDirectory = previousDirectory;
        if (directory != nullptr) {
            VirtualFree(directory, 0, MEM_RELEASE);
        }
        if (state != nullptr) {
            lua_close(state);
        }
    }

    void CheckNativePath(bool initialPath) {
        lua_getglobal(state, "package");
        if (initialPath) {
            lua_pushliteral(state, "original-path");
        } else {
            lua_pushnil(state);
        }
        lua_setfield(state, -2, "cpath");
        lua_pop(state, 1);

        const int stackTop = lua_gettop(state);
        ApplyInjection(state);
        ASSERT_EQ(lua_gettop(state), stackTop);

        lua_getglobal(state, "package");
        lua_getfield(state, -1, "cpath");
        ASSERT_TRUE(lua_isstring(state, -1));
        std::string expected = "C:\\test\\?.dll";
#if defined(PROJECT_ROOT) && defined(_DEBUG)
        expected += ";" PROJECT_ROOT "/?.dll";
#endif
        if (initialPath) {
            expected += ";original-path";
        }
        EXPECT_EQ(expected, lua_tostring(state, -1));
        lua_pop(state, 2);
    }

    lua_State* state = nullptr;
    DllDirectory* directory = nullptr;
    DllDirectory* previousDirectory = nullptr;
};

TEST_F(InjectionPathTest, PreservesExistingNativePaths) {
    CheckNativePath(true);
}

TEST_F(InjectionPathTest, HandlesNilNativePath) {
    CheckNativePath(false);
}

TEST(BuildDllDirectoryTest, Dll) {
    auto h = GetModuleHandleW(L"kernel32.dll");
    auto error = InitializeDllDirectory(reinterpret_cast<uintptr_t>(h));
    ASSERT_EQ(error, ERROR_SUCCESS);
    ASSERT_NE(g_ModuleDirectory, nullptr);
    std::wstring path(g_ModuleDirectory->path, g_ModuleDirectory->length);
    for (auto& c : path) {
        c = static_cast<char>(std::toupper(c));
    }
    ASSERT_EQ(path, L"C:\\WINDOWS\\SYSTEM32");
}