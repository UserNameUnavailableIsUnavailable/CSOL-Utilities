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
#include <iostream>
#include <winreg.h>
#include "CSOL24H_EXCEPT.hpp"
#include "GameState.hpp"
#include "Command.hpp"
#include "Console.hpp"

/* 程序运行标志 */
bool CSOL24H::bInitialize = false;
bool CSOL24H::bDestroy = false;
bool CSOL24H::bAllowExtendedMode = false;
/* 程序运行所需时间信息 */
int64_t CSOL24H::time_bias = 0;
/* 事件句柄 */
HANDLE CSOL24H::hEnableWatchGameStateEvent = NULL;
HANDLE CSOL24H::hEnableWatchGameProcessEvent = NULL;
HANDLE CSOL24H::hEnablePurchaseItemEvent = NULL;
HANDLE CSOL24H::hEnableCombinePartsEvent = NULL;
HANDLE CSOL24H::hEnableLocateCursorEvent = NULL;
/* 互斥量句柄 */
HANDLE CSOL24H::hRunnableMutex = NULL;
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
/* hWatchInGameStateThread 线程所需资源 */
std::shared_ptr<wchar_t[]> CSOL24H::pwszErrorLogFilePath = nullptr;
GameState CSOL24H::game_state(ENUM_GAME_STATE::GS_UNKNOWN, 0);
char* CSOL24H::lpGameErrorLogBuffer = nullptr;
int64_t CSOL24H::log_buffer_last_modified_time = 0;
int64_t CSOL24H::cbGameErrorLogSize = 0;
bool CSOL24H::bGameErrorLogBufferResolved = false;
int64_t CSOL24H::game_error_log_file_date = 0;
/* hWatchGameProcessStateThread 线程所需资源 */
std::shared_ptr<wchar_t[]> CSOL24H::pwsTCGameExePath = nullptr;
std::shared_ptr<wchar_t[]> CSOL24H::pwsTCGRunCSOCmd = nullptr;
HANDLE CSOL24H::hGameProcess = NULL;

void CSOL24H::InitializeWatchInGameStateThread()
{
    /* 用于触发 hWatchGameStateThread 运行的事件 */
    hEnableWatchGameStateEvent = CreateEventW(nullptr, true, false, NULL);
    if (!hEnableWatchGameStateEvent)
    {
        throw CSOL24H_EXCEPT("【错误】创建事件对象 %ls 失败。错误代码 %lu。", L"hEnableWatchGameStateEvent", GetLastError());
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
        throw CSOL24H_EXCEPT("打开文件 %ls 失败，错误代码：%lu。", pwszErrorLogFilePath.get(), GetLastError() );
    }
    /* 创建用于下达命令的 LUA 脚本文件 */
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
        throw CSOL24H_EXCEPT("【错误】打开文件 %ls 失败。错误代码 %lu。", CMD_FILE_NAME, GetLastError());
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
        throw CSOL24H_EXCEPT("【错误】创建用于监测游戏状态的线程时失败。错误代码：%lu。", GetLastError());
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
        throw CSOL24H_EXCEPT("【错误】创建用于处理热键事件的线程时失败。错误代码：%lu。", GetLastError());
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
    ConsoleLog("【消息】TCGame 所在路径：%ls\r\n", pwsTCGameExePath.get());
    /* 启动 CSOL 的命令行 */
    cchSize = cchSize + wcslen(L" cso") + 8; /* "C:\Program Files (x86)\TCGame\TCGame.exe" cso */
    pwsTCGRunCSOCmd = std::shared_ptr<wchar_t[]>(new wchar_t[cchSize]);
    wcscpy_s(pwsTCGRunCSOCmd.get(), cchSize, pwsTCGameExePath.get());
    wcscat_s(pwsTCGRunCSOCmd.get(), cchSize, L" cso");
    ConsoleLog("【消息】启动 CSOL 使用的命令行：%ls\r\n", pwsTCGRunCSOCmd.get());
    hGameProcess = INVALID_HANDLE_VALUE; /* 游戏进程句柄设置为 INVALID_HANDLE_VALUE 将自动创建游戏进程 */
    HWND hTCGWnd = FindWindowW(NULL, L"TCGames");
    if (hTCGWnd)
    {
        ConsoleLog("【消息】检测到 TCGame 正在运行，为防止登录信息失效，将结束 TCGame。\r\n");
        DWORD dwProcessId;
        GetWindowThreadProcessId(hTCGWnd, &dwProcessId);
        HANDLE hTCGProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, dwProcessId);
        if (!hTCGProcess) /* 无法打开句柄 */
        {
            ConsoleLog("【警告】无法获取 TCGame 进程信息。\r\n");
        }
        else if (!TryStopProcessSafely(hTCGProcess)) /* 句柄打开，但无法结束进程 */
        {
            ConsoleLog("【警告】尝试结束 TCGame 失败。TCGame 未结束会导致一段时间后登录信息失效。\r\n");
            CloseHandle(hTCGProcess);
        }
        else /* 打开句柄，且结束进程 */
        {
            CloseHandle(hTCGProcess);
        }
    }
    hEnableWatchGameProcessEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!hEnableWatchGameProcessEvent)
    {
        throw CSOL24H_EXCEPT("【错误】创建事件 %ls 失败。错误代码：%lu。", L"hEnableWatchGameProcessEvent", GetLastError());
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
        throw CSOL24H_EXCEPT("【错误】创建用于监测游戏进程状态的线程时失败。错误代码：%lu。", L"hWatchGameProcessStateThread", GetLastError());
    }
}

void CSOL24H::InitializeCombinePartsThread()
{
    hEnableCombinePartsEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!hEnableCombinePartsEvent)
    {
        throw CSOL24H_EXCEPT("【错误】创建事件 %ls 失败。错误代码：%lu。", L"hEnableCombinePartsEvent", GetLastError());
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
        throw CSOL24H_EXCEPT("【错误】创建合成配件线程失败。错误代码 %lu。", GetLastError());
    }
}

void CSOL24H::InitializePurchaseItemThread()
{
    hEnablePurchaseItemEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!hEnableCombinePartsEvent)
    {
        throw CSOL24H_EXCEPT("【错误】创建事件 %ls 失败。错误代码 %lu。", L"hEnablePurchaseItemEvent", GetLastError());
    }
    hPurchaseItemThread = CreateThread(NULL, 512, PurchaseItem, NULL, 0, NULL);
    if (!hPurchaseItemThread)
    {
        throw CSOL24H_EXCEPT("【错误】创建用于物品购买的线程失败。错误代码 %lu。", GetLastError());
    }
}

void CSOL24H::InitializeLocateCursorThread()
{
    hEnableLocateCursorEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!hEnableCombinePartsEvent)
    {
        throw CSOL24H_EXCEPT("【错误】创建事件 %ls 失败。错误代码 %lu。", L"hEnableLocateCursorThread", GetLastError());
    }
    hLocateCursorThread = CreateThread(NULL, 512, LocateCursor, NULL, 0, NULL);
    if (!hLocateCursorThread)
    {
        throw CSOL24H_EXCEPT("【错误】创建用于获取鼠标光标位置的线程失败。错误代码 %lu。", GetLastError());
    }
}

void CSOL24H::Initialize()
{
    bDestroy = false; /* 该变量设为 true 后，各个 watcher 线程将退出 */
    TIME_ZONE_INFORMATION tzi;
    if (TIME_ZONE_ID_INVALID == GetTimeZoneInformation(&tzi))
    {
        throw CSOL24H_EXCEPT("【错误】获取操作系统时区信息失败。错误代码：%lu。", GetLastError());
    }
    time_bias = tzi.Bias * 60;
    hRunnableMutex = CreateMutexW(NULL, FALSE, NULL);
    if (!hRunnableMutex)
    {
        throw CSOL24H_EXCEPT("【错误】创建互斥体 %ls 失败。错误代码：%lu。", L"hRunnableMutex", GetLastError());
    }
    hGamingToolModule = LoadLibraryW(L"GamingTool.dll");
    if (!hGamingToolModule)
    {
        throw CSOL24H_EXCEPT("【错误】加载模块 GamingTool.dll 时失败。错误代码 %lu。", GetLastError());
    }
    auto InitializeGamingToolDll = (BOOL(*)(void))GetProcAddress(hGamingToolModule, "InitializeGamingToolDll");
    if (!InitializeGamingToolDll || !InitializeGamingToolDll())
    {
        throw CSOL24H_EXCEPT("【错误】初始化模块 GamingTool.dll 时失败。错误代码 %lu。", GetLastError());
    }
    InitializeWatchInGameStateThread();
    InitializeWatchGameProcessThread();
    InitializeCombinePartsThread();
    InitializePurchaseItemThread();
    InitializeLocateCursorThread();
    InitializeHandleHotKeyMessageThread();
    bInitialize = true;
    ConsoleLog("【消息】初始化完成\r\n");
    ConsoleLog("【消息】本集成工具由 _CoreDump 开发。B 站 ID：_CoreDump，联系邮箱：ttyuig@126.com。本工具开源免费，请注意甄别。项目地址：https://gitee.com/silver1867/gaming-tool。\r\n");
}

void CSOL24H::Run()
{
    HANDLE hThreads[] = {
        hHandleHotKeyMessageThread,
        hWatchInGameStateThread,
        hWatchGameProcessStateThread
    };
    DWORD dwSignaledObjectIndex = WaitForMultipleObjects(
        sizeof(hThreads) / sizeof(HANDLE),
        hThreads,
        FALSE,
        INFINITE
    );
    if (dwSignaledObjectIndex >= 0 && dwSignaledObjectIndex <= sizeof(hThreads) / sizeof(HANDLE) - 1 && !bDestroy)
    {
        throw CSOL24H_EXCEPT("【错误】线程异常退出。");
    }
    else if (dwSignaledObjectIndex == WAIT_FAILED)
    {
        throw CSOL24H_EXCEPT("【错误】等待诸线程时发生错误。错误代码：%lu。", GetLastError());
    }
    else if (bDestroy) /* 正在由 Destroy() 销毁，则等待所有线程退出 */
    {
        WaitForMultipleObjects(
            sizeof(hThreads) / sizeof(HANDLE),
            hThreads,
            TRUE,
            INFINITE
        );
        ConsoleLog("【消息】各线程退出。\r\n");
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
        throw CSOL24H_EXCEPT("【错误】获取注册表信息失败。注册表项路径：%s\\%s\\%s。错误代码：%ld。", lpszPredefinedTopDirName, lpSubDir, lpItemName, ret);
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
    SetEvent(hEnableWatchGameStateEvent);
    SetEvent(hEnableWatchGameProcessEvent);
    SetEvent(hEnableCombinePartsEvent);
    SetEvent(hEnablePurchaseItemEvent);
    SetEvent(hEnableLocateCursorEvent);
    if (WAIT_OBJECT_0 != WaitForSingleObject(hWatchInGameStateThread, 2000))
    {
        TerminateThread(hWatchInGameStateThread, -1);
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(hWatchGameProcessStateThread, 5000))
    {
        TerminateThread(hWatchInGameStateThread, -1);
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(hCombinePartsThread, 1500))
    {
        TerminateThread(hCombinePartsThread, -1);
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(hPurchaseItemThread, 1500))
    {
        TerminateThread(hPurchaseItemThread, -1);
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(hLocateCursorThread, 1500))
    {
        TerminateThread(hLocateCursorThread, -1);
    }

    CloseHandle(hEnableWatchGameStateEvent);
    CloseHandle(hEnableWatchGameProcessEvent);
    CloseHandle(hEnableCombinePartsEvent);
    CloseHandle(hEnablePurchaseItemEvent);
    CloseHandle(hEnableLocateCursorEvent);

    CloseHandle(hRunnableMutex);

    CloseHandle(hWatchInGameStateThread);
    CloseHandle(hWatchGameProcessStateThread);
    CloseHandle(hCombinePartsThread);
    CloseHandle(hPurchaseItemThread);
    CloseHandle(hLocateCursorThread);
    CloseHandle(hHandleHotKeyMessageThread);
    GiveCommand(nullptr);
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
    pwsTCGameExePath = nullptr;
    pwsTCGRunCSOCmd = nullptr;
    // TODO: 静态变量重新初始化

}
