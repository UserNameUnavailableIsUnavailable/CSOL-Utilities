#include "Export.h"
#include "WindowUtils.h"

TOOL_API void Setup()
{
    SetupWindowUtils();
}

TOOL_API void Cleanup()
{
    CleanupWindowUtils();
}