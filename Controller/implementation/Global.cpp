#include "Global.hpp"
#include <cstdint>
#include <climits>

namespace CSOL_Utilities
{
    namespace Global
    {
		std::wstring g_LocaleResourcesDirectory = L"locales";
		std::wstring g_LocaleName = L"zh_CN"; /* 区域格式名称 */
		std::wstring g_ExecutorCommandFilePath = L"../Executor/$~cmd.lua"; /* 执行器命令文件路径 */
		std::wstring g_GameRootDirectory; /* 游戏根目录 */
		std::wstring g_GameExecutablePath; /* 游戏可执行文件路径 */
		std::wstring g_LaunchGameCmd; /* 启动游戏命令 */
		std::wstring g_DetectMode = L"OCR"; /* 检测模式 */
		std::wstring g_OCRDetectionModelPath = L"models/OCR/ch_PP-OCRv4_det_infer.onnx"; /* 检测模型路径 */
		std::wstring g_OCRRecognitionModelPath = L"models/OCR/ch_PP-OCRv4_rec_infer.onnx"; /* 识别模型路径 */
		std::wstring g_OCRDictionaryPath = L"models/OCR/dictionary.txt"; /* 字典文件路径 */
		std::wstring g_OCRKeywordsPath = L"models/OCR/keywords.json"; /* 关键词路径 */
		std::wstring g_LGHUB_Agent_Name = L"lghub_agent.exe"; /* LGHUB Agent 进程名称 */
		uint32_t g_StartGameRoomTimeout = 900; /* 在房间内的最长等待时间 */               
		uint32_t g_LoadMapTimeout = UINT32_MAX; /* 在房间内的最长等待时间 */                     
		uint32_t g_LoginTimeout = 5 * 60; /* 登录超时时间 */                                 
		bool g_DefaultIdleAfterReconnection = true; /* 在掉线重连后切换为默认模式 */    
		bool g_RestartGameOnLoadingTimeout = false; /* 如果加载超时则终止游戏进程 */        
		bool g_AllowQuickFullScreen = false; /* 允许 Alt Enter 快捷全屏 */
    }
}
