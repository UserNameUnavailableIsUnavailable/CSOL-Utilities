#pragma once
#include "GameState.hpp"
#include <Windows.h>
#include <cstdint>
#include <memory>
#include <synchapi.h>
#include <winreg.h>
#include <string>
#include "GameState.hpp"

struct MessageTime
{
    int64_t SecondsSince1970;
    int32_t MilliSeconds;
    inline bool operator==(const MessageTime& mt)
    {
        return mt.MilliSeconds == this->MilliSeconds && mt.SecondsSince1970 == this->SecondsSince1970;
    }
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
static bool UpdateErrorLogBuffer() noexcept;
static void DispatchCommand() noexcept;
static int64_t ResolveLogDate(LPCSTR lpBuffer, INT64 cbLength) noexcept;
static int64_t ResolveMessageTimestamp(const std::string message, int32_t* lpMillisecond) noexcept;
static void GiveCommand(const char* cmd) noexcept;
static DWORD CALLBACK WatchInGameState(LPVOID lpParam) noexcept;
static DWORD CALLBACK WatchGameProcess(LPVOID lpParam) noexcept; /* 监视游戏进程状态 */
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
static HANDLE hEnableWatchGameStateEvent; /* 启动 hWatchInGameStateThread 线程的事件 */
static HANDLE hEnableWatchGameProcessEvent; /* 启动 hWatchGameProcessStateThread 线程的事件 */
static HANDLE hEnablePurchaseItemEvent; /* 启动 hPurchaseItemThread 线程的事件 */
static HANDLE hEnableCombinePartsEvent; /* 启动 hCombinePartsThread 线程的事件 */
static HANDLE hEnableLocateCursorEvent; /* 启动 hLocateCursorThread 线程的事件 */
/* 互斥量句柄 */
static HANDLE hRunnableMutex; /* 使线程运行互斥 */
/* 文件句柄 */
static HANDLE hGameErrorLogFile; /* 游戏日志文件 */
static HANDLE hLUACommandFile; /* 向罗技 LUA 语言服务器下达命令的脚本文件 */
/* 线程句柄 */
static HANDLE hWatchInGameStateThread; /* 通过解析日志文件实时监测游戏内状态 */
static HANDLE hWatchGameProcessStateThread; /* 监视游戏进程状态，并在游戏进程退出时重新启动游戏进程 */
static HANDLE hHandleHotKeyMessageThread; /* 绑定、处理热键消息 */
static HANDLE hCombinePartsThread; /* 合成配件 */
static HANDLE hPurchaseItemThread; /* 购买物品 */
static HANDLE hLocateCursorThread; /* 定位光标 */
/* hWatchInGameStateThread 线程所需资源 */
static std::shared_ptr<wchar_t[]> pwszErrorLogFilePath; /* 游戏 Error.log 日志路径 */
static GameState game_state; /* 通过解析日志文件获取到的游戏状态 */
static char* lpGameErrorLogBuffer; /* 游戏日志文件内容读取到此处 */
static uint64_t qwLogBufferLastModifiedTime; /* 游戏日志文件缓冲区修改日期 */
static int64_t cbGameErrorLogSize; /* 日志文件缓冲区当前读入大小 */
static bool bGameErrorLogBufferResolved; /* 日志文件缓冲区在更新后是否被解析过 */
static uint64_t qwGameErrorLogFileDateTime; /* 游戏日志文件中的日期，使用该日 00:00:00 时刻的时间戳表示 */
/* hWatchGameProcessStateThread 线程所需资源 */
static std::shared_ptr<wchar_t[]> pwsTCGameExePath; /* TCGame 游戏启动器路径 */
static std::shared_ptr<wchar_t[]> pwsTCGRunCSOCmd; /* TCGame 启动 CSOL 使用的命令行参数 */
static HANDLE hGameProcess; /* 游戏进程句柄 */
};