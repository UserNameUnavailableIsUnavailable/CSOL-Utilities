#include <Windows.h>
#include <lghubex/dll/hook.h>

#include <gtest/gtest.h>

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