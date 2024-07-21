#include "CSOL24H.hpp"
#include "CSOL24H_EXCEPT.hpp"
#include <cstddef>
#include <handleapi.h>
#include <iostream>
#include <Windows.h>
#include <errhandlingapi.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <winerror.h>
#include <winnt.h>
#include <tlhelp32.h>
#include "Console.hpp"

/*
@brief 尝试以安全方式结束进程。
@param `hProcess` 进程句柄，需要具有 `QUERY_LIMITED_INFORMATION | SYNCHRONIZE` 访问权限。
@param `hThread = NULL` 进程的主线程句柄，默认为 `NULL`。
@return 通过向进程中的所有线程发出 `WM_QUIT` 消息安全地结束进程。如果进程退出成功，则关闭传入的进程和线程句柄并返回 `true`，否则返回 `false`。
*/
bool CSOL24H::TryStopProcessSafely(HANDLE hProcess, HANDLE hThread) noexcept
{
    HANDLE hThreadSnap;
    THREADENTRY32 thread_entry;
    thread_entry.dwSize = sizeof(THREADENTRY32);
    DWORD dwProcessId = GetProcessId(hProcess);
    DWORD dwThreadId = hThread == NULL ? 0 : GetThreadId(hThread);
    /* 首先尝试向主线程发送退出消息 */
    if (hThread != NULL)
    {
        PostThreadMessageW(dwThreadId, WM_QUIT, 0, 0);
        if (WAIT_OBJECT_0 == WaitForSingleObject(hProcess, 2000)) /* 等待 2 s */
        {
            goto close_handles; /* 成功退出 */
        }
    }
    /* 如不成功，再尝试获取进程的所有线程 */
    hThreadSnap = CreateToolhelp32Snapshot(
        TH32CS_SNAPTHREAD, dwProcessId
    ); /* 创建线程 snapshot */
    if (hThreadSnap == INVALID_HANDLE_VALUE) /* 创建 thread snapshot 失败 */
    {
        return false;
    }
    if (!Thread32First(hThreadSnap, &thread_entry))
    {
        CloseHandle(hThreadSnap);
        return false;
    }
    /* 尝试通过向各个线程发送退出消息结束进程 */
    do
    {
        if (thread_entry.th32OwnerProcessID == dwProcessId)
        {
            PostThreadMessageW(thread_entry.th32ThreadID, WM_QUIT, 0, 0); /* 向所有的线程发送 WM_QUIT */
        }
    } while (Thread32Next(hThreadSnap, &thread_entry));
    CloseHandle(hThreadSnap); /* 关闭 snapshot */
    if (WAIT_OBJECT_0 != WaitForSingleObject(hProcess, 2000)) /* 等待 2 s */
    {
        return false;
    }
close_handles:
    CloseHandle(hProcess);
    if (hThread != NULL)
        CloseHandle(hThread);
    return true;
}

DWORD CALLBACK CSOL24H::WatchGameProcess(LPVOID lpParam) noexcept
{
    PROCESS_INFORMATION tcg_process_info;
    ZeroMemory(&tcg_process_info, sizeof(PROCESS_INFORMATION));
    while (WAIT_OBJECT_0 == WaitForSingleObject(hEnableWatchGameProcessEvent, INFINITE))
    {
        /* 是否退出 */
        if (bDestroy)
        {
            break;
        }
        /* 处理无效句柄 */
        if (hGameProcess == NULL) /* 等待游戏进程窗口打开 */
        {
            HWND hWnd = FindWindowW(NULL, L"Counter-Strike Online");
            if (!hWnd)
            {
                Sleep(500);
                continue; /* 游戏窗口未创建 */
            }
            DWORD dwProcessId = 0;
            GetWindowThreadProcessId(hWnd, &dwProcessId);
            if (dwProcessId == 0)
            {
                ConsoleLog("【错误】获取反恐精英 Online 进程标识符时发生错误。错误代码：%lu。\r\n", GetLastError());
                return -1;
            }
            hGameProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, dwProcessId);
            if (!hGameProcess)
            {
                ConsoleLog("【错误】尝试获取反恐精英 Online 进程信息时发生错误。错误代码：%lu。\r\n", GetLastError());
                return -1;
            }
            if (tcg_process_info.dwProcessId != 0)
            {
                if (!TryStopProcessSafely(tcg_process_info.hProcess, tcg_process_info.hThread))
                {
                    ConsoleLog("【警告】尝试结束 TCGame 失败。这会导致一段时间后登录信息失效。\r\n");
                }
                ConsoleLog("【消息】成功结束 TCGame。\r\n");
                ZeroMemory(&tcg_process_info, sizeof(tcg_process_info));
            }
            SetEvent(hEnableWatchGameStateEvent);
            ConsoleLog("【消息】成功获取游戏进程信息。\r\n");
        }
        else if (hGameProcess == INVALID_HANDLE_VALUE) /* 重启游戏 */
        {
            hGameProcess = NULL;
            STARTUPINFOW startup_info_w;
            ZeroMemory(&startup_info_w, sizeof(startup_info_w));
            ZeroMemory(&tcg_process_info, sizeof(tcg_process_info));
            startup_info_w.cb = sizeof(startup_info_w);
            PROCESS_INFORMATION process_information;
            HWND hWnd = FindWindowW(NULL, L"TCGames");
            if (FindWindowW(NULL, L"Counter-Strike Online") || /* CSOL 已经启动 */
                CreateProcessW( /* CSOL 未启动，则运行 CSOL */
                NULL,
                pwsTCGRunCSOCmd.get(),
                NULL,
                NULL,
                FALSE,
                NORMAL_PRIORITY_CLASS,
                NULL,
                L"C:\\WINDOWS\\SYSTEM32",
                &startup_info_w,
                &tcg_process_info
            )) {
                if (hWnd) /* 进程原先已经启动 */
                {
                    CloseHandle(tcg_process_info.hProcess);
                    CloseHandle(tcg_process_info.hThread);
                    ZeroMemory(&tcg_process_info, sizeof(tcg_process_info));
                    ConsoleLog("【消息】TCGame 已经启动。\r\n");
                    GetWindowThreadProcessId(hWnd, &tcg_process_info.dwProcessId);
                    tcg_process_info.hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, tcg_process_info.dwProcessId);
                }
                ConsoleLog("【消息】等待游戏进程启动。\r\n");
            }
            else 
            {
                ConsoleLog("【警告】通过 TCGame 自动创建游戏进程失败。错误代码：%lu。请尝试手动运行游戏。\r\n", GetLastError());
            }
            continue;
        }
        DWORD dwResult = WaitForSingleObject(hGameProcess, 500);
        if (dwResult == WAIT_TIMEOUT) /* 本轮等待超时 */
        {
            continue;
        }
        else if (dwResult == WAIT_OBJECT_0) /* 进程结束 */
        {
            ResetEvent(hEnableWatchGameStateEvent);
            ConsoleLog("【消息】游戏进程结束。执行断线重连。\r\n");
            CloseHandle(hGameProcess); /* 关闭已经退出的的游戏进程句柄 */
            hGameProcess = INVALID_HANDLE_VALUE; /* 句柄初始化为 0 */
        }
        else /* 等待失败 */
        {
            ConsoleLog("【消息】hWatchGameProcess 线程运行遇到错误。错误代码：%lu。\r\n", GetLastError());
            return -1;
        }
        Sleep(1000); /* 每隔 1 秒运行一次本线程 */
    }
    if (tcg_process_info.dwProcessId != 0)
    {
        CloseHandle(tcg_process_info.hProcess);
        CloseHandle(tcg_process_info.hThread);
    }
    ConsoleLog("【消息】线程 hWatchGameProcessThread 退出。\r\n");
    return 0;
}