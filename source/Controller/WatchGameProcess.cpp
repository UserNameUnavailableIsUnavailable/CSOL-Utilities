#include "CController.hpp"
#include "CConsole.hpp"
#include <Windows.h>
#include <chrono>
#include <cstddef>
#include <handleapi.h>
#include <memory>
#include <cwchar>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <thread>
#include <winnt.h>
#include <locale>

using namespace CSOL_Utilities;

void CController::WatchGameProcess() noexcept
{
    thread_local GAME_PROCESS_STATE game_process_state{ GAME_PROCESS_STATE::GPS_UNKNOWN };
    thread_local DWORD dwGameProcessId{ 0 };
    thread_local HANDLE hGameProcess{ nullptr };
    std::setlocale(LC_ALL, ".UTF-8");
    thread_local HWND hCSOBannerWnd{ nullptr };
    CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "游戏启动命令：%ls", s_Instance->m_LaunchGameCmd.get());
    while (true)
    {
        s_Instance->m_GameProcessWatcherSwitch.Wait();
        if (s_Instance->m_ExitThreads) {
            break;
        }
        s_Instance->m_GameProcessWatcherFinished.Reset();
        /* 检测 CSOBanner 是否在运行 */
        hCSOBannerWnd = FindWindowW(nullptr, L"CSOBanner");
        DWORD dwCSOBannerPId{ 0 };
        HANDLE hCSOBannerProcess{ nullptr };
        if (hCSOBannerWnd) {
            GetWindowThreadProcessId(hCSOBannerWnd, &dwCSOBannerPId);
        }
        if (dwCSOBannerPId) {   
            hCSOBannerProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwCSOBannerPId);
        }
        if (hCSOBannerProcess) {
            TerminateProcess(hCSOBannerProcess, -1);
            CloseHandle(hCSOBannerProcess);
        }
        /* 如果本线程曾被挂起（模式切换），挂起后游戏启动，用户手动结束游戏，那么将导致状态机无法更新，故需要按下述方式处理 */
        if (s_Instance->m_bWasGameProcessWatcherInterrupted) {
            if (game_process_state == GAME_PROCESS_STATE::GPS_BEING_CREATED) {
                game_process_state = GAME_PROCESS_STATE::GPS_UNKNOWN;
            }
            s_Instance->m_bWasGameProcessWatcherInterrupted = false;
        }
        if (game_process_state == GAME_PROCESS_STATE::GPS_BEING_CREATED) {
            s_Instance->m_hGameWindow = FindWindowW(nullptr, L"Counter-Strike Online");
            if (!s_Instance->m_hGameWindow) {
                goto stop_this_round;
            }
            GetWindowThreadProcessId(s_Instance->m_hGameWindow, &dwGameProcessId);
            if (dwGameProcessId == 0) {
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, "获取反恐精英 Online 进程标识符时发生错误。错误代码：%lu。", GetLastError());
                break;
            }
            hGameProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE |SYNCHRONIZE, FALSE, dwGameProcessId);
            if (!hGameProcess) {
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, "尝试获取反恐精英 Online 进程信息时发生错误。错误代码：%lu。", GetLastError());
                break;
            }
            game_process_state = GAME_PROCESS_STATE::GPS_RUNNING; /* 跳转执行监测游戏进程运行状态的代码块 */
            CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "成功获取游戏进程信息。进程标识符：%lu。", dwGameProcessId);
            s_Instance->m_GameProcessAlive.Set();
        } else if (game_process_state == GAME_PROCESS_STATE::GPS_RUNNING) {
            DWORD dwResult = WaitForSingleObject(hGameProcess, 0); /* 监测游戏进程状态 */
            if (dwResult == WAIT_OBJECT_0) { /* 进程结束 */
                s_Instance->m_GameProcessAlive.Reset(); /* 暂停监视游戏内状态 */
                game_process_state = GAME_PROCESS_STATE::GPS_EXITED;
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_WARNING, "游戏进程退出，执行掉线重连。");
                CloseHandle(hGameProcess); /* 关闭已经退出的的游戏进程句柄 */
                s_Instance->m_hGameWindow = nullptr;
                hGameProcess = nullptr;
                dwGameProcessId = 0;
            } else if (dwResult == WAIT_TIMEOUT) { /* 本轮等待超时 */
                SetWindowPos(s_Instance->m_hGameWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                goto stop_this_round; /* 不可以简单地使用 continue，否则会导致死锁！ */
            } else { /* 等待失败 */
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, "m_GameProcessWatcher 运行遇到错误。错误代码：%lu。\r\n", GetLastError());
                break;
            }
        } else if (game_process_state == GAME_PROCESS_STATE::GPS_EXITED) { /* 游戏进程退出，重启游戏 */
            s_Instance->m_GameProcessAlive.Reset(); /* 游戏进程结束 */
            STARTUPINFOW si{ .cb = sizeof(STARTUPINFOW) };
            PROCESS_INFORMATION pi{ };
            BOOL bRet = CreateProcessW(
                nullptr,
                s_Instance->m_LaunchGameCmd.get(),
                nullptr,
                nullptr,
                false,
                NORMAL_PRIORITY_CLASS,
                nullptr,
                L"C:\\WINDOWS\\SYSTEM32",
                &si,
                &pi
            );
            if (bRet) {
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "等待游戏进程启动。");
            } else {
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_WARNING, "通过 TCGame 自动创建游戏进程失败。错误代码：%lu。请尝试手动运行游戏。", GetLastError());
            }
            game_process_state = GAME_PROCESS_STATE::GPS_BEING_CREATED; /* 跳转执行等待游戏进程启动的代码块 */
            if (pi.dwProcessId != 0) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
        } else if (game_process_state == GAME_PROCESS_STATE::GPS_UNKNOWN) { /* 游戏进程状态未确定，确定游戏进程状态 */            
            if (hGameProcess) {
                CloseHandle(hGameProcess);
                hGameProcess = nullptr;
            }
            CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "游戏进程状态未知，检测游戏进程状态。");
            s_Instance->m_hGameWindow = FindWindowW(nullptr, L"Counter-Strike Online"); /* 尝试获取游戏进程窗口 */
            if (!s_Instance->m_hGameWindow) {
                game_process_state = GAME_PROCESS_STATE::GPS_EXITED; /* 游戏进程未启动 */
                goto stop_this_round; /* 跳转至执行启动游戏进程的代码块 */
            }
            GetWindowThreadProcessId(s_Instance->m_hGameWindow, &dwGameProcessId);
            if (!dwGameProcessId) {
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, "获取反恐精英 Online 进程标识符时发生错误。错误代码：%lu。", GetLastError());
                break;
            }
            hGameProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, dwGameProcessId);
            if (!hGameProcess) {
                CConsole::Log(CONSOLE_LOG_LEVEL::CLL_ERROR, "尝试获取反恐精英 Online 进程信息时发生错误。错误代码：%lu。", GetLastError());
                break;
            }
            CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "成功获取游戏进程信息。游戏进程标识符：%lu。", dwGameProcessId);
            game_process_state = GAME_PROCESS_STATE::GPS_RUNNING;
            s_Instance->m_GameProcessAlive.Set();
        }
stop_this_round:
        s_Instance->m_GameProcessWatcherFinished.Set();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    if (hGameProcess) {
        CloseHandle(hGameProcess);
        hGameProcess = nullptr;
    }
    CConsole::Log(CONSOLE_LOG_LEVEL::CLL_MESSAGE, "线程 m_GameProcessWatcher 退出。");
    s_Instance->m_ThreadExitEvent.Set();
}