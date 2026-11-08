#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

struct DllDirectory {
    size_t capacity;
    size_t length;
    wchar_t path[];
};

extern struct DllDirectory *g_ModuleDirectory;

uint32_t InitializeDllDirectory(uintptr_t hDll);

struct lua_State;
void ApplyInjection(struct lua_State *L);

typedef uint64_t (*LuaVMInitializer_T)(uint64_t vm, uint64_t param1, uint64_t param2);
int HookLuaVmInitializer(LuaVMInitializer_T fn);

#ifdef __cplusplus
}
#endif