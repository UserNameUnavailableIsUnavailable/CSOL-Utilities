#pragma once

namespace CSOL_Utilities
{
    namespace Global
    {
		extern std::wstring LocaleResourcesDirectory; /* 区域格式资源目录 */
		extern std::wstring LocaleName; /* 区域格式名称 */
		extern std::wstring ExecutorCommandFilePath; /* 执行器命令文件路径 */
		extern std::wstring g_IdleEngineBackbone; /* 检测模式 */
		extern std::wstring DetectMode; /* 检测模式 */
		extern std::wstring GameRootDirectory; /* 游戏根目录 */
		extern std::wstring LaunchGameCmd; /* 启动游戏命令 */
		extern std::wstring OCRDetectionModelPath; /* 检测模型路径 */
		extern std::wstring OCRRecognitionModelPath; /* 识别模型路径 */
		extern std::wstring OCRDictionaryPath; /* 字典文件路径 */
		extern std::wstring OCRKeywordsPath; /* 关键词路径 */
		extern uint32_t StartGameRoomTimeout; /* 在房间内的最长等待时间 */
		extern uint32_t LoadMapTimeout; /* 在房间内的最长等待时间 */
		extern uint32_t LoginTimeout; /* 登录超时时间 */
		extern bool DefaultIdleAfterReconnection; /* 在掉线重连后切换为普通模式 */
		extern bool RestartGameOnLoadingTimeout; /* 如果加载超时则终止游戏进程 */
		extern std::wstring LGHUB_Agent_Name; /* LGHUB Agent 进程名称 */
		extern bool AllowQuickFullScreen; /* 允许 Alt Enter 快捷全屏 */
		extern bool SuppressCSOBanner; /* 自动关闭 CSO Banner */
    }
}
