#include <lghubex/pattern.hpp>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <span>
#include <format>

static constexpr std::optional<std::uint8_t> target_pattern[] = {
    0x4C, 0x89, 0x44, 0x24, std::nullopt, 0x48, 0x89, 0x4C, 0x24, std::nullopt, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8D, 0x6C, 0x24
};

int OnAttach() {
    HANDLE hProcess = GetModuleHandleW(L"lghub_agent.exe");

    if (hProcess == NULL) {
        MessageBoxW(NULL, L"Failed to get module handle for lghub_agent.exe", L"Error", MB_OK | MB_ICONERROR);
        return ERROR_NOT_FOUND;
    }

    std::span<const std::optional<std::uint8_t>> pattern_span(&target_pattern[0], sizeof(target_pattern) / sizeof(target_pattern[0]));
    std::uintptr_t target_address = lghubex::SearchPattern(reinterpret_cast<std::uintptr_t>(hProcess), pattern_span);
    std::wstring message;
    if (target_address == 0)
    {
        message = std::format(L"Failed to locate injection point, please contact the developer: `silver74bh@gmail.com`.", reinterpret_cast<void*>(target_address));
        MessageBoxW(nullptr, message.c_str(), L"Fatal Error", MB_OK);
    }
    #ifdef _DEBUG
    else
    {
        message = std::format(L"Injection point found: `{:p}`.", reinterpret_cast<void*>(target_address));
        MessageBoxW(nullptr, message.c_str(), L"Fatal Error", MB_OK);
    }
    #endif // _DEBUG

    return 0;
}

static DWORD WINAPI Worker(LPVOID parameter) {
    auto module = static_cast<HMODULE>(parameter);
    auto result = OnAttach();
    // MUST not free in the DllMain context. Do it after DllMain has returned.
    FreeLibraryAndExitThread(module, static_cast<DWORD>(result));
}

extern "C" {
    BOOL WINAPI DllMain(
        HINSTANCE hinstDll,
        DWORD fdwReason,
        LPVOID lpvReserved
    ) {
        (void)lpvReserved;
        switch (fdwReason) {
            case DLL_PROCESS_ATTACH: {
                // The worker starts after DllMain releases the loader lock.
                auto thread = CreateThread(nullptr, 0, Worker, hinstDll, 0, nullptr);
                if (!thread) {
                    OutputDebugStringW(L"lghubex: Failed to create attach worker.\n");
                    return FALSE;
                }
                CloseHandle(thread);
                break;
            }
            case DLL_THREAD_ATTACH:
            case DLL_THREAD_DETACH:
            case DLL_PROCESS_DETACH:
                break;
        }
        return TRUE;
    }
}