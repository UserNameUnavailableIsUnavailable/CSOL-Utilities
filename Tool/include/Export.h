#pragma once

#include "Tool.h"

// Check if we are compiling with C++ compiler. If so, use C linkage for the exported functions.
#ifdef __cplusplus
extern "C"
{
#endif

TOOL_API void Setup(); // initialize, called after dll is loaded
TOOL_API void Cleanup(); // cleanup, called before dll is uninstalled

#ifdef __cplusplus
}
#endif