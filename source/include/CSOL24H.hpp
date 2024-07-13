#pragma once
#include "GameState.hpp"
#include <Windows.h>
#include <cstdint>
#include <memory>
#include <winreg.h>
#include <string>
#include "GameState.hpp"

// enum ENUM_CSOL_GAME_COMMAND
// {
//     CSOL_NOP, // 此时无需任何操作
//     CSOL_QUERY_CURSOR_LOCATION, // 获取鼠标光标位置，需要打开罗技控制台查看
//     CSOL_ROUND_WAIT, // 在房间等待
//     CSOL_ROUND_LOAD, // 游戏正在加载
//     CSOL_ROUND_CHOOSE_CLASS, // 选择
//     CSOL_ROUND_PLAY, // 游戏正在进行
//     CSOL_ROUND_CONFIRM, // 游戏结算（若成功结算，则转入 CSOL_ROUND_WAIT）
//     CSOL_ROUND_TIMEOUT, // 超时机制
//     CSOL_CRAFT_PARTS_COMBINE, // 制造所合成配件
//     CSOL_STORE_PURCHASE, // 商店购买物品
// };

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
static std::shared_ptr<wchar_t[]> QueryInstallationPath(HKEY predefinedTopDir = HKEY_CURRENT_USER,
    LPCWSTR lpSubDir = L"Software\\TCGame\\csol",
    LPCWSTR lpItemName = L"gamepath"
    );
static void Initialize();
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
static void StartWatch() noexcept;
static void StopWatch() noexcept;
private:
static bool IsErrorLogFileModified() noexcept;
static bool UpdateErrorLogBuffer() noexcept;
static void DispatchCommand() noexcept;
static int64_t ResolveLogDate(LPCSTR lpBuffer, INT64 cbLength) noexcept;
static int64_t ResolveMessageTimestamp(const std::string message, int32_t* lpMillisecond) noexcept;
static void GiveCommand(const char* cmd) noexcept;
static DWORD CALLBACK WatchGameState(LPVOID lpParam) noexcept;
static void TransferGameState() noexcept;
// static DWORD CALLBACK StartGameRoom(LPVOID lpParam) noexcept;
// static DWORD CALLBACK PlayGame(LPVOID lpParam) noexcept;
// static DWORD CALLBACK ConfirmGame(LPVOID lpParam) noexcept;
static void WatchGameProcess() noexcept; /* 监视游戏进程状态 */
static DWORD HandleHotKey(LPVOID lpParam) noexcept; /* 监视热键 */
CSOL24H() = delete;
~CSOL24H() = delete;
CSOL24H(const CSOL24H& object) = delete;
CSOL24H(const CSOL24H&& object) = delete;
private:
static bool initialized; /* 是否初始化 */
static int64_t time_bias; /* 世界标准时间与本地时间之差 UTC - localtime，单位为秒 */
// static HANDLE hStartGameEvent; /* the event that triggers the game to start */
// static HANDLE hPlayGameEvent; /* the event that causes the lua script to play game */
// static HANDLE hConfirmRoundEvent; /* the event that causes the lua script to confirm a round */
static HANDLE hGameWatcherEvent; /* the event that causes the game watcher thread to start */
static HANDLE hRunnableMutex; /* 使 GameWatcher 线程与其他线程互斥运行 */
static HANDLE hGameStateWatcherThread; /* watch game state real-time */
static HANDLE hGameProcessWatcherThread; /* 监视游戏进程状态 */
static HANDLE hHotKeyEventHandlerThread; /* 处理热键消息 */
// static HANDLE hStartGameThread;
// static HANDLE hPlayGameThread;
// static HANDLE hConfirmGameThread;
static HANDLE hExecLogFile;
static HANDLE hCmdFile;
static HANDLE hErrorLogFile;
static char* lpErrorLogBuffer;
static int64_t cbErrorLogSize;
static uint64_t qwLogBufferLastModifiedTime;
static bool bLogBufferResolved;
static uint64_t qwLogFileDate;
static bool bExit; /* will cause the process to exit */

static GameState game_state;
static std::shared_ptr<wchar_t[]> ErrorLogFilePath;
// static const char* ERROR_LOG_MSG_PATTERN;
// static const char* ERROR_LOG_TIME_PATTERN;
// static const char* ERROR_LOG_DATE_PATTERN;
};