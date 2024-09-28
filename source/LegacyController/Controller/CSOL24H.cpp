#include "CSOL24H.hpp"
#include <Windows.h>
#include <cstddef>
#include <cwchar>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <libloaderapi.h>
#include <memory>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <sec_api/string_s.h>
#include <sec_api/wchar_s.h>
#include <synchapi.h>
#include <timezoneapi.h>
#include <winnt.h>
#include <winreg.h>
#include "CSOL24H_EXCEPT.hpp"
#include "InGameState.hpp"
#include "Command.hpp"
#include "Console.hpp"

/* 程序运行标志 */
bool CSOL24H::bInitialize = false;
bool CSOL24H::bDestroy = false;
bool CSOL24H::bAllowExtendedMode = false;
/* 程序运行所需时间信息 */
int64_t CSOL24H::time_bias = 0;
/* 事件句柄 */
HANDLE CSOL24H::hEnableWatchInGameStateEvent = NULL;
HANDLE CSOL24H::hEnableWatchGameProcessEvent = NULL;
HANDLE CSOL24H::hGameProcessRunningEvent = NULL;
HANDLE CSOL24H::hEnablePurchaseItemEvent = NULL;
HANDLE CSOL24H::hEnableCombinePartsEvent = NULL;
HANDLE CSOL24H::hEnableLocateCursorEvent = NULL;
/* 互斥量句柄 */
HANDLE CSOL24H::hCmdFileMutex = NULL;
/* 文件句柄 */
HANDLE CSOL24H::hGameErrorLogFile = INVALID_HANDLE_VALUE;
HANDLE CSOL24H::hLUACommandFile = INVALID_HANDLE_VALUE;
/* GamingTool 模块 */
HMODULE CSOL24H::hGamingToolModule = NULL;
/* 用于屏蔽系统热键的键盘钩子 */
HHOOK CSOL24H::hLLKH;
/* 线程句柄 */
HANDLE CSOL24H::hWatchInGameStateThread = NULL;
HANDLE CSOL24H::hWatchGameProcessStateThread = NULL;
HANDLE CSOL24H::hHandleHotKeyMessageThread = NULL;
HANDLE CSOL24H::hCombinePartsThread = NULL;
HANDLE CSOL24H::hPurchaseItemThread = NULL;
HANDLE CSOL24H::hLocateCursorThread = NULL;
HANDLE CSOL24H::hStopCSOBannerThread = NULL;
/* hWatchInGameStateThread 线程所需资源 */
std::shared_ptr<wchar_t[]> CSOL24H::pwszErrorLogFilePath = nullptr;
InGameState CSOL24H::in_game_state(ENUM_IN_GAME_STATE::IGS_UNKNOWN, 0);
char* CSOL24H::lpGameErrorLogBuffer = nullptr;
int64_t CSOL24H::log_buffer_last_modified_time = 0;
int64_t CSOL24H::cbGameErrorLogBufferSize = 0;
bool CSOL24H::bGameErrorLogBufferResolved = false;
int64_t CSOL24H::game_error_log_file_date = 0;
int64_t CSOL24H::last_return_to_room_timestamp = 0; /* 上一次由于异常原因回到游戏房间内的时间戳 */
int32_t CSOL24H::return_to_room_times = 0; /* 因网络问题返回到游戏房间的次数 */
/* hWatchGameProcessStateThread 线程所需资源 */
bool CSOL24H::bWasWatchGameProcessStateThreadPaused = false;
std::shared_ptr<wchar_t[]> CSOL24H::pwsTCGameExePath = nullptr;
std::shared_ptr<wchar_t[]> CSOL24H::pwsTCGRunCSOCmd = nullptr;
HANDLE CSOL24H::hGameProcess = NULL;
DWORD CSOL24H::dwGameProcessId = 0;
HWND CSOL24H::hGameWindow = NULL;
ENUM_GAME_PROCESS_STATE CSOL24H::game_process_state = ENUM_GAME_PROCESS_STATE::GPS_UNKNOWN;

void CSOL24H::InitializeWatchInGameStateThread()
{
    /* 用于触发 hWatchGameStateThread 运行的事件 */
    hEnableWatchInGameStateEvent = CreateEventW(nullptr, true, false, NULL);
    if (!hEnableWatchInGameStateEvent)
    {
        throw CSOL24H_EXCEPT("创建事件对象 %ls 失败。错误代码 %lu。", L"hEnableWatchGameStateEvent", GetLastError());
    }
    lpGameErrorLogBuffer = new char[256 * 1024 * 1024];
    /* 获取 Error.log 路径 */
    std::shared_ptr<wchar_t[]> gameInstallationPath = CSOL24H::QueryRegistryStringItem(); /* 查询 CSOL 安装路径 */
    size_t error_log_path_length = 1 + wcslen(gameInstallationPath.get()) + wcslen(L"\\bin\\Error.log");
    pwszErrorLogFilePath = std::shared_ptr<wchar_t[]>(new wchar_t[error_log_path_length]);
    wcscpy_s(pwszErrorLogFilePath.get(), error_log_path_length, gameInstallationPath.get());
    wcscat_s(pwszErrorLogFilePath.get(), error_log_path_length, L"\\bin\\Error.log");
    /* 打开 Error.log 文件 */
    hGameErrorLogFile = CreateFileW(
        pwszErrorLogFilePath.get(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    if (hGameErrorLogFile == INVALID_HANDLE_VALUE)
    {
        throw CSOL24H_EXCEPT("尝试打开日志文件失败，错误代码：%lu。这可能是因游戏安装后尚未运行过导致。", pwszErrorLogFilePath.get(), GetLastError() );
    }
    /* 创建用于下达命令的 Lua 脚本文件 */
    hLUACommandFile = CreateFileW(
        CMD_FILE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS, /* create (not exists) or clear this file (already exists) */
        FILE_ATTRIBUTE_HIDDEN, /* hidden file */
        nullptr
    );
    if (hLUACommandFile == INVALID_HANDLE_VALUE)
    {
        throw CSOL24H_EXCEPT("打开文件 %ls 失败。错误代码 %lu。", CMD_FILE_NAME, GetLastError());
    }
    /* 创建 hWatchGameStateThread 线程 */
    hWatchInGameStateThread = CreateThread(
        nullptr,
        4096,
        CSOL24H::WatchInGameState,
        nullptr,
        0, /* create running */
        nullptr
    );
    if (!hWatchInGameStateThread)
    {
        throw CSOL24H_EXCEPT("创建用于监测游戏状态的线程时失败。错误代码：%lu。", GetLastError());
    }
}

void CSOL24H::InitializeHandleHotKeyMessageThread()
{
    hHandleHotKeyMessageThread = CreateThread(
        nullptr,
        4096,
        CSOL24H::HandleHotKey,
        nullptr,
        0,
        nullptr
    );
    if (!hHandleHotKeyMessageThread)
    {
        throw CSOL24H_EXCEPT("创建用于处理热键事件的线程时失败。错误代码：%lu。", GetLastError());
    }
}

void CSOL24H::InitializeWatchGameProcessThread()
{
    /* 获取 TCGame 可执行文件路径 */
    auto setup = QueryRegistryStringItem(HKEY_CURRENT_USER, L"Software\\TCGame", L"setup"); /* C:\Program Files (x86)\TCGame */
    uint32_t cchSize = wcslen(setup.get()) + wcslen(L"\\TCGame.exe") + 8; /* C:\Program Files (x86)\TCGame\TCGame.exe */
    pwsTCGameExePath = std::shared_ptr<wchar_t[]>(new wchar_t[cchSize]);
    wcscpy_s(pwsTCGameExePath.get(), cchSize, L"\""); /* " */
    wcscat_s(pwsTCGameExePath.get(), cchSize, setup.get()); /* "C:\Program Files (x86)\TCGame */
    wcscat_s(pwsTCGameExePath.get(), cchSize, L"\\TCGame.exe"); /* "C:\Program Files (x86)\TCGame\TCGame.exe */
    wcscat_s(pwsTCGameExePath.get(), cchSize, L"\""); /* "C:\Program Files (x86)\TCGame\TCGame.exe" */
    ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "TCGame 所在路径：%ls", pwsTCGameExePath.get());
    /* 启动 CSOL 的命令行 */
    cchSize = cchSize + wcslen(L" cso") + 8; /* "C:\Program Files (x86)\TCGame\TCGame.exe" cso */
    pwsTCGRunCSOCmd = std::shared_ptr<wchar_t[]>(new wchar_t[cchSize]);
    wcscpy_s(pwsTCGRunCSOCmd.get(), cchSize, pwsTCGameExePath.get());
    wcscat_s(pwsTCGRunCSOCmd.get(), cchSize, L" cso");
    ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "启动 CSOL 使用的命令行：%ls", pwsTCGRunCSOCmd.get());
    game_process_state = ENUM_GAME_PROCESS_STATE::GPS_UNKNOWN; /* 游戏进程状态设定为未知 */
    hEnableWatchGameProcessEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!hEnableWatchGameProcessEvent)
    {
        throw CSOL24H_EXCEPT("创建事件 %ls 失败。错误代码：%lu。", ENUM_CONSOLE_LOG_LEVEL::CLL_ERROR, L"hEnableWatchGameProcessEvent", GetLastError());
    }
    hGameProcessRunningEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!hGameProcessRunningEvent)
    {
        throw CSOL24H_EXCEPT("创建事件 %ls 失败。错误代码 %lu。", ENUM_CONSOLE_LOG_LEVEL::CLL_ERROR, L"hGameProcessRunningEvent", GetLastError());
    }
    hWatchGameProcessStateThread = CreateThread(
        NULL,
        4096,
        WatchGameProcess,
        NULL,
        0,
        NULL
    );
    if (!hWatchGameProcessStateThread)
    {
        throw CSOL24H_EXCEPT("创建用于监测游戏进程状态的线程时失败。错误代码：%lu。", L"hWatchGameProcessStateThread", GetLastError());
    }
    hStopCSOBannerThread = CreateThread(
        NULL,
        4096,
        StopCSOBanner,
        NULL,
        0,
        NULL
    );
    if (!hStopCSOBannerThread)
    {
        throw CSOL24H_EXCEPT("创建用于关闭 CSOBanner 的线程时失败。错误代码：%lu。", L"hStopCSOBannerThread", GetLastError());
    }
}

void CSOL24H::InitializeCombinePartsThread()
{
    hEnableCombinePartsEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!hEnableCombinePartsEvent)
    {
        throw CSOL24H_EXCEPT("创建事件 %ls 失败。错误代码：%lu。", L"hEnableCombinePartsEvent", GetLastError());
    }
    hCombinePartsThread = CreateThread(
        NULL,
        512,
        CombineParts,
        NULL,
        0,
        NULL
    );
    if (!hCombinePartsThread)
    {
        throw CSOL24H_EXCEPT("创建合成配件线程失败。错误代码 %lu。", GetLastError());
    }
}

void CSOL24H::InitializePurchaseItemThread()
{
    hEnablePurchaseItemEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!hEnableCombinePartsEvent)
    {
        throw CSOL24H_EXCEPT("创建事件 %ls 失败。错误代码 %lu。", L"hEnablePurchaseItemEvent", GetLastError());
    }
    hPurchaseItemThread = CreateThread(NULL, 512, PurchaseItem, NULL, 0, NULL);
    if (!hPurchaseItemThread)
    {
        throw CSOL24H_EXCEPT("创建用于物品购买的线程失败。错误代码 %lu。", GetLastError());
    }
}

void CSOL24H::InitializeLocateCursorThread()
{
    hEnableLocateCursorEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!hEnableCombinePartsEvent)
    {
        throw CSOL24H_EXCEPT("创建事件 %ls 失败。错误代码 %lu。", L"hEnableLocateCursorThread", GetLastError());
    }
    hLocateCursorThread = CreateThread(NULL, 512, LocateCursor, NULL, 0, NULL);
    if (!hLocateCursorThread)
    {
        throw CSOL24H_EXCEPT("创建用于获取鼠标光标位置的线程失败。错误代码 %lu。", GetLastError());
    }
}

void CSOL24H::Initialize()
{
    bDestroy = false; /* 该变量设为 true 后，各个 watcher 线程将退出 */
    TIME_ZONE_INFORMATION tzi;
    if (TIME_ZONE_ID_INVALID == GetTimeZoneInformation(&tzi))
    {
        throw CSOL24H_EXCEPT("获取操作系统时区信息失败。错误代码：%lu。", GetLastError());
    }
    time_bias = tzi.Bias * 60;
    hCmdFileMutex = CreateMutexW(NULL, FALSE, NULL);
    if (!hCmdFileMutex)
    {
        throw CSOL24H_EXCEPT("创建互斥体 %ls 失败。错误代码：%lu。", L"hCmdFileMutex", GetLastError());
    }
    hGamingToolModule = LoadLibraryW(L"GamingTool.dll");
    if (!hGamingToolModule)
    {
        throw CSOL24H_EXCEPT("加载模块 GamingTool.dll 时失败。错误代码 %lu。", GetLastError());
    }
    auto InitializeGamingToolDll = (BOOL(*)(void))GetProcAddress(hGamingToolModule, "InitializeGamingToolDll");
    if (!InitializeGamingToolDll || !InitializeGamingToolDll())
    {
        throw CSOL24H_EXCEPT("初始化模块 GamingTool.dll 时失败。错误代码 %lu。", GetLastError());
    }
    InitializeWatchGameProcessThread();
    InitializeWatchInGameStateThread();
    InitializeCombinePartsThread();
    InitializePurchaseItemThread();
    InitializeLocateCursorThread();
    InitializeHandleHotKeyMessageThread();
    bInitialize = true;
    ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "初始化完成");
    ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "本集成工具由 _CoreDump 开发。B 站 ID：_CoreDump，联系邮箱：ttyuig@126.com。本工具开源免费，请注意甄别。项目地址：https://gitee.com/silver1867/csol-24-h。");
}

void CSOL24H::Run()
{
    HANDLE hThreadsToWait[] = {
        hHandleHotKeyMessageThread,
        hWatchGameProcessStateThread,
        hWatchInGameStateThread,
        hCombinePartsThread,
        hPurchaseItemThread,
        hLocateCursorThread,
        hStopCSOBannerThread
    };
    DWORD dwSignaledObjectIndex = WaitForMultipleObjects(
        ARRAYSIZE(hThreadsToWait),
        hThreadsToWait,
        FALSE,
        INFINITE
    );
    if (dwSignaledObjectIndex >= 0 && dwSignaledObjectIndex <= ARRAYSIZE(hThreadsToWait) - 1 && !bDestroy)
    {
        throw CSOL24H_EXCEPT("线程异常退出。");
    }
    else if (dwSignaledObjectIndex == WAIT_FAILED)
    {
        throw CSOL24H_EXCEPT("等待诸线程时发生错误。错误代码：%lu。", GetLastError());
    }
    else if (bDestroy) /* 正在由 Destroy() 销毁，则等待所有线程退出 */
    {
        WaitForMultipleObjects(
            ARRAYSIZE(hThreadsToWait),
            hThreadsToWait,
            TRUE,
            INFINITE
        );
        ConsoleLog(ENUM_CONSOLE_LOG_LEVEL::CLL_MESSAGE, "各线程退出。");
    }
}

/*
* @brief 获取字符串型注册表项。
* @param `lpPredefinedTopItem` 注册表预定义的项，如 `HKEY_CURRENT_USER`。
* @param `lpSubItem` 注册表子目录项。
* @param `lpItemName` 目录中的项名。
* @return 项的值。
* @note 例如，CSOL 安装路径保存在注册表项 HKEY_CURRENT_USER\Software\TCGame\csol\gamepath 中，则 `hPredefinedTopDir = HKEY_CURRENT_USER`，`lpSubDir = Software\TCGame`，`lpItemName = gamepath`。
*/
std::shared_ptr<wchar_t[]> CSOL24H::QueryRegistryStringItem(HKEY hPredefinedTopDir, LPCWSTR lpSubDir, LPCWSTR lpItemName)
{
    DWORD cbRequiredBufferSize;
    const char* lpszPredefinedTopDirName = "";
    if (hPredefinedTopDir == HKEY_CLASSES_ROOT)
    {
        lpszPredefinedTopDirName = "HKEY_CLASSES_ROOT";
    }
    else if (hPredefinedTopDir == HKEY_CURRENT_CONFIG)
    {
        lpszPredefinedTopDirName = "HKEU_CURRENT_CONFIG";
    }
    else if (hPredefinedTopDir == HKEY_CURRENT_USER)
    {
        lpszPredefinedTopDirName = "HKEY_CURRENT_USER";
    }
    else if (hPredefinedTopDir == HKEY_LOCAL_MACHINE)
    {
        lpszPredefinedTopDirName = "HKEY_LOCAL_MACHINE";
    }
    else if (hPredefinedTopDir == HKEY_USERS)
    {
        lpszPredefinedTopDirName = "HKEY_USERS";
    }
    else
    {
        lpszPredefinedTopDirName = "UNKNOWN";
    }
    RegGetValueW(
        hPredefinedTopDir,
        lpSubDir,
        lpItemName,
        RRF_RT_REG_SZ,
        nullptr,
        nullptr,
        &cbRequiredBufferSize
    ); // 获取需要的缓冲区长度
    std::shared_ptr<wchar_t[]> lpwszResult(new wchar_t[(cbRequiredBufferSize - 1) / 2 + 1]{0});
    LSTATUS ret = RegGetValueW(
        hPredefinedTopDir,
        lpSubDir,
        lpItemName,
        RRF_RT_REG_SZ,
        nullptr,
        lpwszResult.get(),
        &cbRequiredBufferSize
    ); // 读取字符串
    if (ret != ERROR_SUCCESS)
    {
        throw CSOL24H_EXCEPT("获取注册表信息失败。注册表项路径：%s\\%s\\%s。错误代码：%ld。", lpszPredefinedTopDirName, lpSubDir, lpItemName, ret);
    }
    return lpwszResult;
}

void CSOL24H::Destroy() noexcept
{
    if (!bInitialize)
        return;
    bInitialize = false;
    bDestroy = true;
    /* 先结束热键处理，避免释放事件时产生并发问题 */
    PostThreadMessage(GetThreadId(hHandleHotKeyMessageThread), WM_QUIT, 0, 0);
    if (WAIT_OBJECT_0 != WaitForSingleObject(hHandleHotKeyMessageThread, 500))
    {
        TerminateThread(hHandleHotKeyMessageThread, -1);
    }
    SetEvent(hEnableWatchGameProcessEvent);
    if (WAIT_OBJECT_0 != WaitForSingleObject(hWatchGameProcessStateThread, 5000))
    {
        TerminateThread(hWatchInGameStateThread, -1);
    }
    SetEvent(hEnableWatchInGameStateEvent);
    SetEvent(hGameProcessRunningEvent);
    if (WAIT_OBJECT_0 != WaitForSingleObject(hWatchInGameStateThread, 2000))
    {
        TerminateThread(hWatchInGameStateThread, -1);
    }
    SetEvent(hEnableCombinePartsEvent);
    if (WAIT_OBJECT_0 != WaitForSingleObject(hCombinePartsThread, 2000))
    {
        TerminateThread(hCombinePartsThread, -1);
    }
    SetEvent(hEnablePurchaseItemEvent);
    if (WAIT_OBJECT_0 != WaitForSingleObject(hPurchaseItemThread, 2000))
    {
        TerminateThread(hPurchaseItemThread, -1);
    }
    SetEvent(hEnableLocateCursorEvent);
    if (WAIT_OBJECT_0 != WaitForSingleObject(hLocateCursorThread, 2000))
    {
        TerminateThread(hLocateCursorThread, -1);
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(hStopCSOBannerThread, 2000))
    {
        TerminateThread(hStopCSOBannerThread, -1);
    }

    CloseHandle(hEnableWatchInGameStateEvent);
    CloseHandle(hEnableWatchGameProcessEvent);
    CloseHandle(hEnableCombinePartsEvent);
    CloseHandle(hEnablePurchaseItemEvent);
    CloseHandle(hEnableLocateCursorEvent);

    GiveCommand(nullptr);
    CloseHandle(hCmdFileMutex);

    CloseHandle(hStopCSOBannerThread);
    CloseHandle(hWatchInGameStateThread);
    CloseHandle(hWatchGameProcessStateThread);
    CloseHandle(hGameProcessRunningEvent);
    CloseHandle(hCombinePartsThread);
    CloseHandle(hPurchaseItemThread);
    CloseHandle(hLocateCursorThread);
    CloseHandle(hHandleHotKeyMessageThread);
    CloseHandle(hLUACommandFile);
    CloseHandle(hGameErrorLogFile);

    if (hGameProcess != NULL && hGameProcess != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hGameProcess);
    }

    delete[] lpGameErrorLogBuffer;
    auto DeinitializeGamingToolDll = (void(*)(void))GetProcAddress(hGamingToolModule, "DeinitializeGamingTool");
    if (DeinitializeGamingToolDll) DeinitializeGamingToolDll();
    FreeLibrary(hGamingToolModule);   
    /* 静态变量初始化 */
    /* 程序运行标志 */
    CSOL24H::bInitialize = false;
    CSOL24H::bDestroy = false;
    CSOL24H::bAllowExtendedMode = false;
    /* 程序运行所需时间信息 */
    CSOL24H::time_bias = 0;
    /* 事件句柄 */
    CSOL24H::hEnableWatchInGameStateEvent = NULL;
    CSOL24H::hEnableWatchGameProcessEvent = NULL;
    CSOL24H::hGameProcessRunningEvent = NULL;
    CSOL24H::hEnablePurchaseItemEvent = NULL;
    CSOL24H::hEnableCombinePartsEvent = NULL;
    CSOL24H::hEnableLocateCursorEvent = NULL;
    /* 互斥量句柄 */
    CSOL24H::hCmdFileMutex = NULL;
    /* 文件句柄 */
    CSOL24H::hGameErrorLogFile = INVALID_HANDLE_VALUE;
    CSOL24H::hLUACommandFile = INVALID_HANDLE_VALUE;
    /* GamingTool 模块 */
    CSOL24H::hGamingToolModule = NULL;
    /* 用于屏蔽系统热键的键盘钩子 */
    CSOL24H::hLLKH = NULL;
    /* 线程句柄 */
    CSOL24H::hWatchInGameStateThread = NULL;
    CSOL24H::hWatchGameProcessStateThread = NULL;
    CSOL24H::hHandleHotKeyMessageThread = NULL;
    CSOL24H::hCombinePartsThread = NULL;
    CSOL24H::hPurchaseItemThread = NULL;
    CSOL24H::hLocateCursorThread = NULL;
    CSOL24H::hStopCSOBannerThread = NULL;
    /* hWatchInGameStateThread 线程所需资源 */
    CSOL24H::pwszErrorLogFilePath = nullptr;
    CSOL24H::in_game_state.update(ENUM_IN_GAME_STATE::IGS_UNKNOWN, 0);
    CSOL24H::lpGameErrorLogBuffer = nullptr;
    CSOL24H::log_buffer_last_modified_time = 0;
    CSOL24H::cbGameErrorLogBufferSize = 0;
    CSOL24H::bGameErrorLogBufferResolved = false;
    CSOL24H::game_error_log_file_date = 0;
    CSOL24H::last_return_to_room_timestamp = 0; /* 上一次由于异常原因回到游戏房间内的时间戳 */
    CSOL24H::return_to_room_times = 0; /* 因网络问题返回到游戏房间的次数 */
    /* hWatchGameProcessStateThread 线程所需资源 */
    CSOL24H::bWasWatchGameProcessStateThreadPaused = false;
    CSOL24H::pwsTCGameExePath = nullptr;
    CSOL24H::pwsTCGRunCSOCmd = nullptr;
    CSOL24H::hGameProcess = NULL;
    CSOL24H::dwGameProcessId = 0;
    CSOL24H::hGameWindow = NULL;
    CSOL24H::game_process_state = ENUM_GAME_PROCESS_STATE::GPS_UNKNOWN;
}
