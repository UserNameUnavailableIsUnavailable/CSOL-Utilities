#pragma once

#include <string>
#include <cstdint>

namespace CSOL_Utilities
{
    namespace Global
    {
		extern std::wstring g_LocaleResourcesDirectory; /* 区域格式资源目录 */
		extern std::wstring g_LocaleName; /* 区域格式名称 */
		extern std::wstring g_ExecutorCommandFilePath; /* 执行器命令文件路径 */
		extern std::wstring g_IdleEngineBackbone; /* 检测模式 */
		extern std::wstring g_DetectMode; /* 检测模式 */
		extern std::wstring g_GameRootDirectory; /* 游戏根目录 */
		extern std::wstring g_GameExecutablePath; /* 游戏可执行文件路径 */
		extern std::wstring g_LaunchGameCmd; /* 启动游戏命令 */
		extern std::wstring g_OCRDetectionModelPath; /* 检测模型路径 */
		extern std::wstring g_OCRRecognitionModelPath; /* 识别模型路径 */
		extern std::wstring g_OCRDictionaryPath; /* 字典文件路径 */
		extern std::wstring g_OCRKeywordsPath; /* 关键词路径 */
		extern uint32_t g_StartGameRoomTimeout; /* 在房间内的最长等待时间 */
		extern uint32_t g_LoadMapTimeout; /* 在房间内的最长等待时间 */
		extern uint32_t g_LoginTimeout; /* 登录超时时间 */
		extern bool g_DefaultIdleAfterReconnection; /* 在掉线重连后切换为普通模式 */
		extern bool g_RestartGameOnLoadingTimeout; /* 如果加载超时则终止游戏进程 */
		extern std::wstring g_LGHUB_Agent_Name; /* LGHUB Agent 进程名称 */
		extern bool g_AllowQuickFullScreen; /* 允许 Alt Enter 快捷全屏 */
    }
}
