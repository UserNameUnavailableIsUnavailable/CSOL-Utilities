#pragma once

#include <Windows.h>
#include <cstdint>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "Command.hpp"
#include "Event.hpp"

namespace CSOL_Utilities
{
	namespace Shared
	{
		extern ExecutorCommand g_Command; /* 命令对象 */
	}
	namespace Opt
	{
		extern std::string g_DetectMode; /* 检测模式 */
		extern std::string g_GameRootDirectory; /* 游戏根目录 */
		extern std::string g_LaunchGameCmd; /* 启动游戏命令 */
		extern std::string g_OCRDetectionModelPath; /* 检测模型路径 */
		extern std::string g_OCRRecognitionModelPath; /* 识别模型路径 */
		extern std::string g_OCRDictionaryPath; /* 字典文件路径 */
		extern std::string g_OCRKeywordsPath; /* 关键词路径 */
		extern unsigned int g_MaxWaitTimeStartGameRoom; /* 在房间内的最长等待时间 */
		extern unsigned int g_MaxWaitTimeLogin; /* 在房间内的最长等待时间 */
		extern unsigned int g_MaxWaitTimeLoading; /* 在房间内的最长等待时间 */
		extern bool g_bSwitchToNormalModeAfterReconnection; /* 在掉线重连后切换为普通模式 */
		extern bool g_bRestartGameIfLoadingTimeout; /* 如果加载超时则终止游戏进程 */
		extern std::string g_LGHUB_Agent_Name; /* LGHUB Agent 进程名称 */
		extern std::string g_GameProcessDetectorName; /* 游戏进程探测器名称 */
		extern std::string g_IdleEngineName; /* 挂机引擎名称 */
		extern std::string g_CommandDispatcherName; /* 命令派发器名称 */
	} // namespace Opt
	enum class EXCEPTION_LEVEL
	{
		EL_MESSAGE,
		EL_WARNING,
		EL_DEBUG,
		EL_ERROR
	};

	/* 游戏状态 */
	enum class IN_GAME_STATE
	{
		IGS_LOGIN, /* 正在登陆 */
		IGS_LOBBY, /* 在大厅中 */
		IGS_ROOM, /* 在房间内 */
		IGS_LOADING, /* 游戏场景正在加载 */
		IGS_GAMING, /* 在游戏地图中 */
		IGS_UNKNOWN /* 未知状态 */
	};
	enum class GAME_PROCESS_STATE
	{
		GPS_BEING_CREATED, /* 游戏进程正在被创建 */
		GPS_RUNNING, /* 游戏进程正在运行 */
		GPS_EXITED, /* 游戏进程退出 */
		GPS_UNKNOWN, /* 尚未确认游戏进程状态 */
	};
	using BITMAPBODY = uint8_t;
	std::wstring ConvertUtf8ToUtf16(const std::string& u8);
	std::string ConvertUtf16ToUtf8(const std::wstring& u16);
	std::string QueryRegistryString(HKEY hPredefinedTopDir, LPCWSTR lpSubDir, LPCWSTR lpItemName);
	BOOL IsRunningAsAdmin() noexcept;
	std::filesystem::path GetModulePath(uintptr_t hMod);
	DWORD GetProcessIdByNameW(const std::wstring& process_name);
	void CaptureWindowAsBmp(HWND hWnd, std::vector<uint8_t>& buffer);
} // namespace CSOL_Utilities
