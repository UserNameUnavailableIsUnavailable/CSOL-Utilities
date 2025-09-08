#include "Global.hpp"

namespace CSOL_Utilities
{
    namespace Global
    {
		std::wstring LocaleResourcesDir = L"Locales";
		std::wstring LocaleName = L"zh-CN"; /* 区域格式名称 */
		std::wstring ExecutorCommandFilePath = L"Temporary.lua"; /* 执行器命令文件路径 */
		std::wstring GameRootDir; /* 游戏根目录 */
		std::wstring LaunchGameCmd; /* 启动游戏命令 */
		std::wstring IdleEngineType = L"Classifier"; /* 检测模式 */
		std::wstring OCRDetectorJSONPath = L"Models\\OCR\\Chinese_Simplified\\detector.json"; /* 检测模型路径 */
		std::wstring OCRRecognizerJSONPath = L"Models\\OCR\\Chinese_Simplified\\recognizer.json"; /* 识别模型路径 */
		std::wstring OCRKeywordsJSONPath = L"Models\\OCR\\Chinese_Simplified\\keywords.json"; /* 关键词路径 */
		std::wstring ClassifierModelJSONPath = L"Models\\Classifier\\ResNet\\CSOL-Utilities-ResNet18-800x600.json"; /* 图像分类模型路径 */
		std::wstring LGHUBAgentName = L"lghub_agent.exe"; /* LGHUB Agent 进程名称 */
		uint32_t StartGameRoomTimeout = 900; /* 在房间内的最长等待时间 */               
		uint32_t LoadMapTimeout = UINT32_MAX; /* 在房间内的最长等待时间 */                     
		uint32_t LoginTimeout = 5 * 60; /* 登录超时时间 */                                 
		uint32_t MaxInGameTime = UINT32_MAX; /* 最长游戏内时间 */
		bool DefaultIdleAfterReconnection = true; /* 在掉线重连后切换为默认模式 */    
		bool RestartGameOnLoadingTimeout = false; /* 如果加载超时则终止游戏进程 */        
		bool AllowQuickFullScreen = false; /* 允许 Alt Enter 快捷全屏 */
		bool SuppressCSOBanner = false; /* 自动关闭 CSO Banner */
    }
}
