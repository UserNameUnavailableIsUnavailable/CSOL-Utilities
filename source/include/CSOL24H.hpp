#pragma once
#include <Windows.h>
#include <cstdint>
#include <memory>
#include <synchapi.h>
#include <windef.h>
#include <winreg.h>
#include <string>
#include "InGameState.hpp"

enum ENUM_GAME_PROCESS_STATE
{
    GPS_BEING_CREATED, /* 游戏进程正在被创建 */
    GPS_RUNNING, /* 游戏进程正在运行 */
    GPS_EXITED, /* 游戏进程退出 */
    GPS_UNKNOWN, /* 尚未确认游戏进程状态 */
};

class CSOL24H
{
public:
static std::shared_ptr<wchar_t[]> QueryRegistryStringItem(HKEY predefinedTopDir = HKEY_CURRENT_USER,
    LPCWSTR lpSubDir = L"Software\\TCGame\\csol",
    LPCWSTR lpItemName = L"gamepath"
    );
static void Initialize();
static void Run();
static void Destroy() noexcept;
static uint64_t GetUNIXTimestamp(
    uint32_t dwYear = 0,
    uint32_t dwMonth = 0,
    uint32_t dwDay = 0,
    uint32_t dwHour = 0,
    uint32_t dwMinute = 0,
    uint32_t dwSecond = 0,
    float fTimeZone = 0
    );
static inline void EnableExtendedMode() noexcept { bAllowExtendedMode = true; };
static inline void DisableExtendedMode() noexcept {bAllowExtendedMode = false; }
private:
static void InitializeWatchInGameStateThread();
static void InitializeHandleHotKeyMessageThread();
static void InitializeCombinePartsThread();
static void InitializePurchaseItemThread();
static void InitializeLocateCursorThread();
static void InitializeWatchGameProcessThread();
static bool IsErrorLogFileModified() noexcept;
static void UpdateErrorLogBuffer();
static void DispatchCommand() noexcept;
[[deprecated("从日志文件中获取到的日期在日期变更（23:59 之后）后可能不准确。")]] static int64_t ResolveLogDate(LPCSTR lpBuffer, INT64 cbLength) noexcept;
static int64_t ResolveLogDate(FILETIME filetime) noexcept;
static int64_t ResolveMessageTimestamp(const std::string message, int32_t* lpMillisecond) noexcept;
static void GiveCommand(const char* cmd) noexcept;
static DWORD CALLBACK WatchInGameState(LPVOID lpParam) noexcept;
static DWORD CALLBACK WatchGameProcess(LPVOID lpParam) noexcept; /* 监视游戏进程状态 */
static DWORD CALLBACK StopCSOBanner(LPVOID lpParam) noexcept;
static void ResolveGameStateFromErrorLog() noexcept;
static void MaintainGameState() noexcept;
static void TransferGameState() noexcept;
static DWORD HandleHotKey(LPVOID lpParam) noexcept; /* 监视热键 */
static DWORD CombineParts(LPVOID lpParam) noexcept; /* 合成配件 */
static DWORD LocateCursor(LPVOID lpParam) noexcept; /* 定位光标 */
static DWORD PurchaseItem(LPVOID lpParam) noexcept; /* 购买物品 */
static bool TryStopProcessSafely(HANDLE hProcess, HANDLE hThread = NULL) noexcept;
CSOL24H() = delete;
~CSOL24H() = delete;
CSOL24H(const CSOL24H& object) = delete;
CSOL24H(const CSOL24H&& object) = delete;
private:
/* 程序运行标志 */
static bool bInitialize; /* 是否初始化 */
static bool bDestroy; /* 退出 CSOL24H */
static bool bAllowExtendedMode; /* 是否允许以扩展模式挂机 */
/* 程序运行所需的时间信息 */
static int64_t time_bias; /* 世界标准时间与本地时间之差 UTC - localtime，单位为秒 */
/* 事件句柄 */
static HANDLE hEnableWatchInGameStateEvent; /* 启动 hWatchInGameStateThread 线程的事件 */
static HANDLE hEnableWatchGameProcessEvent; /* 启动 hWatchGameProcessStateThread 线程的事件 */
static HANDLE hGameProcessRunningEvent; /* 游戏进程运行事件 */
static HANDLE hEnablePurchaseItemEvent; /* 启动 hPurchaseItemThread 线程的事件 */
static HANDLE hEnableCombinePartsEvent; /* 启动 hCombinePartsThread 线程的事件 */
static HANDLE hEnableLocateCursorEvent; /* 启动 hLocateCursorThread 线程的事件 */
/* 互斥量句柄 */
static HANDLE hCmdFileMutex; /* 使文件写入互斥 */
/* 文件句柄 */
static HANDLE hGameErrorLogFile; /* 游戏日志文件 */
static HANDLE hLUACommandFile; /* 向罗技 LUA 语言服务器下达命令的脚本文件 */
/* GamingTool 模块句柄 */
static HMODULE hGamingToolModule;
/* 用于屏蔽系统热键的键盘钩子 */
static HHOOK hLLKH;
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
/* 线程句柄 */
static HANDLE hWatchInGameStateThread; /* 通过解析日志文件实时监测游戏内状态 */
static HANDLE hWatchGameProcessStateThread; /* 监视游戏进程状态，并在游戏进程退出时重新启动游戏进程 */
static HANDLE hHandleHotKeyMessageThread; /* 绑定、处理热键消息 */
static HANDLE hCombinePartsThread; /* 合成配件 */
static HANDLE hPurchaseItemThread; /* 购买物品 */
static HANDLE hLocateCursorThread; /* 定位光标 */
static HANDLE hStopCSOBannerThread; /* 定期结束 CSOBanner */
/* hWatchInGameStateThread 线程所需资源 */
static std::shared_ptr<wchar_t[]> pwszErrorLogFilePath; /* 游戏 Error.log 日志路径 */
static InGameState in_game_state; /* 通过解析日志文件获取到的游戏状态 */
static char* lpGameErrorLogBuffer; /* 游戏日志文件内容读取到此处 */
static int64_t cbGameErrorLogBufferSize; /* 当前缓冲区中的日志字符串字节数 */
static int64_t log_buffer_last_modified_time; /* 游戏日志文件缓冲区修改时间（WINDOWS FILETIME） */
static bool bGameErrorLogBufferResolved; /* 日志文件缓冲区在更新后是否被解析过 */
static int64_t game_error_log_file_date; /* 游戏日志文件中的日期，使用该日 00:00:00 时刻的 UNIX 时间戳表示（UTC 时间） */
static int64_t last_return_to_room_timestamp; /* 上一次由于异常原因回到游戏房间内的时间戳 */
static int32_t return_to_room_times; /* 因网络问题返回到游戏房间的次数 */
/* hWatchGameProcessStateThread 线程所需资源 */
static bool bWasWatchGameProcessStateThreadPaused; /* hWatchGameProcessStateThread 是否被挂起过 */
static std::shared_ptr<wchar_t[]> pwsTCGameExePath; /* TCGame 游戏启动器路径 */
static std::shared_ptr<wchar_t[]> pwsTCGRunCSOCmd; /* TCGame 启动 CSOL 使用的命令行参数 */
static HANDLE hGameProcess; /* 游戏进程句柄 */
static DWORD dwGameProcessId; /* 游戏进程标识符 */
static HWND hGameWindow; /* 游戏窗口句柄 */
static ENUM_GAME_PROCESS_STATE game_process_state; /* 游戏进程状态 */
};