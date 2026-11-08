#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <assert.h>
#include <MinHook.h>

#include <lghubex/dll/features.h>
#include <lghubex/dll/hook.h>

const size_t LUA_STATE_OFFSET = 0x30;
static uint64_t InitializeLuaVM(uint64_t vm, uint64_t param1, uint64_t param2);
static LuaVMInitializer_T g_RawInitializer = NULL;

struct DllDirectory *g_ModuleDirectory = NULL;

uint32_t InitializeDllDirectory(uintptr_t hDll) {
    size_t capacity = 512;
    size_t length = 0;
    while (!g_ModuleDirectory)
    {
        g_ModuleDirectory = VirtualAlloc(
            NULL,
            sizeof(struct DllDirectory) + capacity * sizeof(wchar_t),
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE
        );
        if (!g_ModuleDirectory) {
            return GetLastError();
        }
        length = GetModuleFileNameW(
            (HMODULE)hDll,
            g_ModuleDirectory->path,
            (DWORD)capacity
        );
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            VirtualFree(g_ModuleDirectory, 0, MEM_RELEASE);
            g_ModuleDirectory = NULL;
            capacity *= 2;
            continue;
        } else if (length == 0) {
            VirtualFree(g_ModuleDirectory, 0, MEM_RELEASE);
            g_ModuleDirectory = NULL;
            return GetLastError();
        } else {
            break;
        }
    }
    wchar_t *p = g_ModuleDirectory->path + length - 1;
    while (p > g_ModuleDirectory->path && *p != L'\\')
    {
        --p;        
    }
    *p = L'\0';
    length = p - g_ModuleDirectory->path;
    g_ModuleDirectory->capacity = capacity;
    g_ModuleDirectory->length = length;
    return ERROR_SUCCESS;
}

int HookLuaVmInitializer(LuaVMInitializer_T origin)
{
    LuaVMInitializer_T backup = NULL;
    if (MH_Initialize() != MH_OK)
    {
        return -1;
    }
    
    if (MH_CreateHook((void *)origin, (void *)&InitializeLuaVM, (void **)&backup) != MH_OK)
    {
        return -1;
    }

    if (MH_EnableHook((void *)origin) != MH_OK)
    {
        MH_RemoveHook((void *)origin);
        return -1;
    }

    g_RawInitializer = backup;
    return 0; // Return the original function pointer as the backup
}

// Must be called after `package` module is loaded.
static void PrependModulePath(lua_State *L, const char *path)
{
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    const char *current_path = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_pushfstring(L, "%s;%s", path, current_path);
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);
}

void ApplyInjection(lua_State *L)
{
    if (L == NULL)
    {
        return;
    }
    RestoreBuiltinModules(L);
    RestoreBuiltinFunctions(L);

    #if defined(PROJECT_ROOT) && defined(_DEBUG)
    char debug_import_path[] = PROJECT_ROOT "/?.lua";
    PrependModulePath(L, debug_import_path);
    char debug_init_path[] = PROJECT_ROOT "/?/init.lua";
    PrependModulePath(L, debug_init_path);
    #endif

    if (g_ModuleDirectory)
    {
        wchar_t *p = g_ModuleDirectory->path + g_ModuleDirectory->length - 1;
        while (p > g_ModuleDirectory->path && *p != L'\\')
        {
            --p;        
        }
        // C:\path\to\module\directory\lghubex
        //                            ^
        //                            p
        size_t dir_length = p - g_ModuleDirectory->path;
        const char init[] = "\\?\\init.lua";
        const char import[] = "\\?.lua";

        // WideCharToMultiByte does not null-terminate an output string if the input string length is explicitly specified without a terminating null character.
        size_t required = WideCharToMultiByte(CP_UTF8, 0, g_ModuleDirectory->path, (int)dir_length, NULL, 0, NULL, NULL);

        required += sizeof(init); // required now includes null-terminator

        char *u8 = (char *)malloc(required);
        if (u8)
        {
            size_t length = WideCharToMultiByte(CP_UTF8, 0, g_ModuleDirectory->path, (int)dir_length, u8, (int)required, NULL, NULL);
            memcpy(u8 + length, import, sizeof(import));
            PrependModulePath(L, u8);
            memcpy(u8 + length, init, sizeof(init));
            PrependModulePath(L, u8);
            free(u8);
        }
    }
}

uint64_t InitializeLuaVM(uint64_t vm, uint64_t param1, uint64_t param2)
{
    assert(g_RawInitializer != NULL);
    uint64_t res = g_RawInitializer(vm, param1, param2);
    lua_State *L = *(lua_State **)(vm + LUA_STATE_OFFSET);
    ApplyInjection(L);
    return res;
}