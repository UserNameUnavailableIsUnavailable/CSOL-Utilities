#include "CSOL24H.hpp"
#include <Windows.h>
#include <cstddef>
#include <errhandlingapi.h>
#include <handleapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <timezoneapi.h>
#include <winnt.h>
#include <iostream>
#include "CSOL24H_EXCEPT.hpp"
#include "GameState.hpp"
#include "Command.hpp"


int64_t CSOL24H::time_bias = 0;
bool CSOL24H::initialized = false;
// HANDLE CSOL24H::hStartGameEvent = NULL;
// HANDLE CSOL24H::hPlayGameEvent = NULL;
// HANDLE CSOL24H::hConfirmRoundEvent = NULL;
HANDLE CSOL24H::hGameWatcherEvent = NULL;
HANDLE CSOL24H::hRunnableMutex = NULL;
// HANDLE CSOL24H::hGiveCmdMutex = NULL;
HANDLE CSOL24H::hGameStateWatcherThread = NULL;
HANDLE CSOL24H::hGameProcessWatcherThread = NULL;
HANDLE CSOL24H::hHotKeyEventHandlerThread = NULL;
// HANDLE CSOL24H::hStartGameThread = NULL;
// HANDLE CSOL24H::hPlayGameThread = NULL;
// HANDLE CSOL24H::hConfirmGameThread = NULL;
HANDLE CSOL24H::hExecLogFile = INVALID_HANDLE_VALUE;
HANDLE CSOL24H::hCmdFile = INVALID_HANDLE_VALUE;
HANDLE CSOL24H::hErrorLogFile = INVALID_HANDLE_VALUE;
int64_t CSOL24H::cbErrorLogSize = 0;
uint64_t CSOL24H::qwLogBufferLastModifiedTime = 0;
bool CSOL24H::bLogBufferResolved = false;
uint64_t CSOL24H::qwLogFileDate = 0;
bool CSOL24H::bExit = false;
GameState CSOL24H::game_state(ENUM_GAME_STATE::GS_UNKNOWN, 0);
std::shared_ptr<wchar_t[]> CSOL24H::ErrorLogFilePath = nullptr;
char* CSOL24H::lpErrorLogBuffer = nullptr;
// const char* CSOL24H::ERROR_LOG_MSG_PATTERN = "(\\d{1,2}):(\\d{2}):(\\d{2}).(\\d{3}) - (.+)"; /* 匹配日志中的消息，另包含消息时间 */
// const char* CSOL24H::ERROR_LOG_TIME_PATTERN = "(\\d{1,2}):(\\d{2}):(\\d{2})\\.(\\d{3})"; /* 匹配日志中的时间 */
// const char* CSOL24H::ERROR_LOG_DATE_PATTERN = "\\d{1,2}:\\d{2}:\\d{2}\\.\\d{3}.+?(\\d{2})/(\\d{2})/(\\d{4})"; /* 匹配日志文件的日期 */

void CSOL24H::Initialize()
{
    TIME_ZONE_INFORMATION tzi;
    if (TIME_ZONE_ID_INVALID == GetTimeZoneInformation(&tzi))
    {
        throw CSOL24H_EXCEPT("【错误】获取操作系统时区信息失败。错误代码：%lu。", GetLastError());
    }
    time_bias = tzi.Bias * 60;
    hCmdFile = CreateFileW(
        CMD_FILE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS, /* create (not exists) or clear this file (already exists) */
        FILE_ATTRIBUTE_HIDDEN, /* hidden file */
        nullptr
    );
    if (hCmdFile == INVALID_HANDLE_VALUE)
    {
        throw CSOL24H_EXCEPT("【错误】打开文件 %ls 失败。错误代码 %lu。", CMD_FILE_NAME, GetLastError());
    }
    hRunnableMutex = CreateMutexW(NULL, FALSE, L"Local\\hRunnableMutex");
    if (!hRunnableMutex)
    {
        throw CSOL24H_EXCEPT("【错误】创建互斥体 %ls 失败。错误代码：%lu。", L"Local\\hRunnableMutex", GetLastError());
    }
    bExit = false;
    hGameWatcherEvent = CreateEventW(nullptr, true, false, L"Local\\hGameWatcherEvent");
    if (!hGameWatcherEvent)
    {
        throw CSOL24H_EXCEPT("【错误】创建事件对象 %ls 失败。错误代码 %lu。", L"Local\\hGameWatcherEvent", GetLastError());
    }
    // hStartGameEvent = CreateEventW(nullptr, false, false, L"Local\\hStartGameEvent");
    // hPlayGameEvent = CreateEventW(nullptr, false, false, L"Local\\hPlayGameEvent");
    // hConfirmRoundEvent = CreateEventW(nullptr, false, false, L"Local\\hConfirmRoundEvent");
    // hGiveCmdMutex = CreateMutexW(nullptr, false, L"Local\\hGiveCmdMutex");
    // if (
        // !hStartGameEvent || !hStartGameEvent || !hPlayGameEvent || !hConfirmRoundEvent ||
        // !hGiveCmdMutex
    // )
    // {
    //     std::printf("创建事件失败，错误代码：%lu。\r\n", GetLastError());
    //     ExitProcess(-1);
    // }
    // TODO: error handling
    lpErrorLogBuffer = new char[256 * 1024 * 1024];
    hGameStateWatcherThread = CreateThread(
        nullptr,
        4096,
        CSOL24H::WatchGameState,
        nullptr,
        0, /* create running */
        nullptr
    );
    if (!hGameStateWatcherThread)
    {
        throw CSOL24H_EXCEPT("【错误】创建用于监测游戏状态的线程时失败。错误代码：%lu。", GetLastError());
    }
    if (WAIT_OBJECT_0 == WaitForSingleObject(hGameStateWatcherThread, 0))
    {
        throw CSOL24H_EXCEPT("【错误】线程 hGameStateWatcherThread 异常退出。");
    }
    hHotKeyEventHandlerThread = CreateThread(
        nullptr,
        4096,
        CSOL24H::HandleHotKey,
        nullptr,
        0,
        nullptr
    );
    if (!hHotKeyEventHandlerThread)
    {
        throw CSOL24H_EXCEPT("【错误】创建用于处理热键事件的线程时失败。错误代码：%lu。", GetLastError());
    }
    if (WAIT_OBJECT_0 == WaitForSingleObject(hHotKeyEventHandlerThread, 100))
    {
        throw CSOL24H_EXCEPT("【错误】绑定热键失败。错误代码：%lu。", GetLastError());
    }
    // hStartGameThread = CreateThread(
    //     nullptr,
    //     4096,
    //     CSOL24H::StartGameRoom,
    //     hStartGameEvent,
    //     0, /* create running */
    //     nullptr
    // );
    // hPlayGameThread = CreateThread(
    //     nullptr,
    //     4096,
    //     PlayGame,
    //     nullptr,
    //     0, /* create running */
    //     nullptr
    // );
    // hConfirmGameThread = CreateThread(
    //     nullptr,
    //     4096,
    //     ConfirmGame,
    //     nullptr,
    //     0, /* create running */
    //     nullptr
    // );
    // TODO: error handling

    /* Get the location of Error.log */
    std::shared_ptr<wchar_t[]> gameInstallationPath = CSOL24H::QueryInstallationPath(); /* 查询 CSOL 安装路径 */
    size_t error_log_path_length = 1 + wcslen(gameInstallationPath.get()) + wcslen(L"\\bin\\Error.log"); /* length of error.log path */
    ErrorLogFilePath = std::shared_ptr<wchar_t[]>(new wchar_t[error_log_path_length]);
    wcscpy_s(ErrorLogFilePath.get(), error_log_path_length, gameInstallationPath.get());
    wcscat_s(ErrorLogFilePath.get(), error_log_path_length, L"\\bin\\Error.log");
    hErrorLogFile = CreateFileW(
        ErrorLogFilePath.get(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    if (hErrorLogFile == INVALID_HANDLE_VALUE)
    {
        throw CSOL24H_EXCEPT("打开文件 %ls 失败，错误代码：%lu。", ErrorLogFilePath.get(), GetLastError() );
    }
    initialized = true;
    HANDLE hThreads[] = {
        hHotKeyEventHandlerThread,
        hGameStateWatcherThread
    };
    std::cout << "【消息】初始化完成" << std::endl;
    WaitForMultipleObjects(sizeof(hThreads) / sizeof(HANDLE), hThreads, TRUE, INFINITE);
}

/*
* CSOL_QueryInstallationPath 获取 CSOL 安装路径
* @param lpPredefinedTopItem 注册表预定义的项
* @param lpSubItem 注册表子项
* @return CSOL 安装路径
* @note CSOL 安装路径保存在注册表项 HKEY_CURRENT_USER\Software\TCGame\csol\gamepath 中，返回的字符串在堆中
*/
std::shared_ptr<wchar_t[]> CSOL24H::QueryInstallationPath(HKEY hPredefinedTopDir, LPCWSTR lpSubDir, LPCWSTR lpItemName)
{
    DWORD dwBufferSize;

    RegGetValueW(
        hPredefinedTopDir,
        lpSubDir,
        lpItemName,
        RRF_RT_REG_SZ,
        nullptr,
        nullptr,
        &dwBufferSize
    ); // 获取需要的缓冲区长度
    std::shared_ptr<wchar_t[]> game_installation_path(new wchar_t[(dwBufferSize - 1) / 2 + 1]{0});
    LSTATUS ret = RegGetValueW(
        hPredefinedTopDir,
        lpSubDir,
        lpItemName,
        RRF_RT_REG_SZ,
        nullptr,
        game_installation_path.get(),
        &dwBufferSize
    ); // 读取字符串
    if (ret != ERROR_SUCCESS)
    {
        throw CSOL24H_EXCEPT(u8"获取注册表信息失败。错误代码：%ld。", ret);
    }
    return game_installation_path;
}

void CSOL24H::Destroy() noexcept
{
    if (!initialized)
        return;
    bExit = true;
    // SetEvent(hStartGameEvent);
    // SetEvent(hPlayGameEvent);
    // SetEvent(hConfirmRoundEvent);
    SetEvent(hGameWatcherEvent);
    if (WAIT_OBJECT_0 != WaitForSingleObject(hGameStateWatcherThread, 2000))
    {
        TerminateThread(hGameStateWatcherThread, -1);
    }
    PostThreadMessage(GetThreadId(hHotKeyEventHandlerThread), WM_QUIT, 0, 0);
    if (WAIT_OBJECT_0 != WaitForSingleObject(hGameStateWatcherThread, 2000))
    {
        TerminateThread(hGameStateWatcherThread, -1);
    }
    // if (WAIT_OBJECT_0 != WaitForSingleObject(hStartGameThread, 1000))
    // {
    //     TerminateThread(hStartGameThread, -1);
    // }
    // if (WAIT_OBJECT_0 != WaitForSingleObject(hPlayGameThread, 1000))
    // {
    //     TerminateThread(hPlayGameThread, -1);
    // }
    // if (WAIT_OBJECT_0 != WaitForSingleObject(hConfirmGameThread, 1000))
    // {
    //     TerminateThread(hConfirmGameThread, -1);
    // }
    // CloseHandle(hStartGameEvent);
    // CloseHandle(hPlayGameEvent);
    // CloseHandle(hConfirmRoundEvent);
    CloseHandle(hGameWatcherEvent);
    CloseHandle(hGameStateWatcherThread);
    CloseHandle(hHotKeyEventHandlerThread);
    // CloseHandle(hStartGameThread);
    // CloseHandle(hPlayGameThread);
    // CloseHandle(hConfirmGameThread);
    CloseHandle(hCmdFile);
    CloseHandle(hErrorLogFile);
    delete[] lpErrorLogBuffer;
    // TODO: 静态变量重新初始化
    initialized = false;
    time_bias = 0;
}
