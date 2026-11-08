#include <lghubex/dll/pattern.h>
#include <lghubex/dll/hook.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

static const int16_t target_pattern[] = {
    0x4C, 0x89, 0x44, 0x24, -1, 0x48, 0x89, 0x4C, 0x24, -1, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8D, 0x6C, 0x24
};

DWORD WINAPI InitializeHook(HMODULE hDll) {
    DWORD error = ERROR_SUCCESS;
    error = (DWORD)InitializeDllDirectory((uintptr_t)hDll);
    if (error != ERROR_SUCCESS) {
        return error;
    }
    
    HMODULE agent_module = GetModuleHandleW(L"lghub_agent.exe");

    if (agent_module == NULL) {
        return GetLastError();
    }

    const uint8_t *text_section_base = NULL;
    size_t text_section_size = 0;
    error = GetTextSection((uintptr_t)agent_module, &text_section_base, &text_section_size);
    if (error != ERROR_SUCCESS) {
        return error;
    }

    const uint8_t *pattern_address = SearchPattern(text_section_base, text_section_size, target_pattern, sizeof(target_pattern) / sizeof(target_pattern[0]));
    if (pattern_address == NULL) {
        return ERROR_NOT_FOUND;
    }

    if (0 != HookLuaVmInitializer((LuaVMInitializer_T)pattern_address)) {
        return ERROR_DLL_INIT_FAILED;
    }

    return ERROR_SUCCESS;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDll,
    DWORD fdwReason,
    LPVOID lpvReserved
) {
    (void)lpvReserved;
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDll);
        InitializeHook(hinstDll);
    } else if (fdwReason == DLL_PROCESS_DETACH) {
        if (g_ModuleDirectory) {
            VirtualFree(g_ModuleDirectory, 0, MEM_RELEASE);
        }
    }
    return TRUE;
}