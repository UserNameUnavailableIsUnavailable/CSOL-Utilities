#pragma once

#include <Windows.h>
#include "Tool.h"

#ifdef __cplusplus
extern "C"
{
#endif
TOOL_API void SetupWindowUtils(); // 初始化窗口工具
TOOL_API void CleanupWindowUtils(); // 清理窗口工具
TOOL_API void MinimizeForegroundWindow(); // 最小化前台窗口
TOOL_API void RestoreMinimizedWindow(); // 恢复最小化窗口
TOOL_API void ChangeForegroundWindowInputLanguage(); // 切换前台窗口输入语言
TOOL_API int IsTopmost(HWND hWnd); // 判断窗口是否置顶
TOOL_API void TopmostWindow(HWND hWnd); // 置顶窗口
TOOL_API void UntopmostWindow(HWND hWnd); // 取消置顶窗口
TOOL_API void ToggleTopmostWindow(HWND hWnd); // 切换前台窗口置顶状态
TOOL_API void ToggleTray(); // 显示/隐藏任务栏
TOOL_API void RemoveWindowBorder(HWND hWnd); // 移除窗口边框
TOOL_API void RestoreWindowBorder(HWND hWnd); // 恢复窗口边框
TOOL_API void CenterWindow(HWND hWnd); // 将窗口居中
TOOL_API void CenterClient(HWND hWnd); // 将窗口客户区居中
TOOL_API void StartCursorClipper(); // 启动光标限制器
TOOL_API void StopCursorClipper(); // 停止光标限制器
TOOL_API void ToggleCursorClipper(); // 切换光标限制器状态

#ifdef __cplusplus
}
#endif