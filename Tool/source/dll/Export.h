#pragma once

#include "Tool.h"
#include "WindowUtils.h"

#ifdef __cplusplus
extern "C"
{
#endif

TOOL_API void Setup(); // 初始化，加载 dll 后调用
TOOL_API void Cleanup(); // 清理，卸载 dll 前调用

#ifdef __cplusplus
}
#endif