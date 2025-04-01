#include <Windows.h>
#include <memory>
#include <string>
#include "Console.hpp"
#include "CSOL_Utilities.hpp"
#include "GameProcessDetector.hpp"
#include "MessageLiterals.hpp"
#include "Exception.hpp"

namespace CSOL_Utilities
{
    void GameProcessDetector::work()
    {
        thread_local GAME_PROCESS_STATE state = GAME_PROCESS_STATE::GPS_UNKNOWN; // 初始状态未知
        thread_local DWORD dwGameProcessId = 0; // 游戏进程标识符

        using namespace MessageLiterals::GAME_PROCESS_DETECTOR;

        if (state == GAME_PROCESS_STATE::GPS_RUNNING) // 游戏进程正在运行
        {
            DWORD dwStatus = WaitForSingleObject(m_hGameProcess.get(), 0);
            if (dwStatus == WAIT_OBJECT_0) // 游戏进程退出
            {
                state = GAME_PROCESS_STATE::GPS_EXITED;
                Console::Log(EXCEPTION_LEVEL::EL_WARNING, MSG_GAME_PROCESS_EXIT);
                disable("InGameStateRecognizer");
            }
        }
        else if (state == GAME_PROCESS_STATE::GPS_BEING_CREATED) // 游戏进程状态未知，或正在创建
        {
            dwGameProcessId = GetProcessIdByName(m_game_process_name);
            if (dwGameProcessId) // 查找到有效的 Id
            {
                m_hGameProcess.reset(
                    OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, dwGameProcessId)
                ); // 通过 Id 打开进程
                if (m_hGameProcess)
                {
                    Console::Log(EXCEPTION_LEVEL::EL_MESSAGE, FMT_SUCCESSFULLY_OPEN_GAME_PROCESS, dwGameProcessId);
                    enable("InGameStateRecognizer");
                    state = GAME_PROCESS_STATE::GPS_RUNNING; // 状态修改为正在运行
                }
                else
                {
                    throw Exception(FMT_FAILED_TO_OPEN_GAME_PROCESS, GetLastError());
                }
            }
        }
        else if (state == GAME_PROCESS_STATE::GPS_EXITED) // 游戏进程退出，需要重启
        {
            PROCESS_INFORMATION pi;
            STARTUPINFOW si;
            BOOL bRet = CreateProcessW(NULL, m_LaunchCmd.get(), NULL,NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, L"C:\\WINDOWS\\SYSTEM32", &si, &pi);
            // si 和 pi 是游戏启动器进程的句柄，故直接关闭
            if (bRet)
            {
                CloseHandle(&pi.hProcess);
                CloseHandle(&pi.hThread);
            }
            else
            {
                throw Exception(FMT_FAILED_TO_CREATE_GAME_PROCESS, GetLastError()); // 自动创建游戏进程可能会失败，这一般是由于没有以管理员权限运行控制器
            }
            state = GAME_PROCESS_STATE::GPS_BEING_CREATED; // 游戏进程正在创建
        }
        else if (state == GAME_PROCESS_STATE::GPS_UNKNOWN)
        {
            dwGameProcessId = GetProcessIdByName(m_game_process_name);
            if (dwGameProcessId)
            {
                Console::Log(EXCEPTION_LEVEL::EL_WARNING, FMT_GAME_PROCESS_DETECTED, dwGameProcessId);
                m_hGameProcess.reset( 
                    OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, dwGameProcessId)
                ); // 通过 Id 打开进程
                if (!m_hGameProcess)
                {
                    throw Exception(FMT_FAILED_TO_OPEN_GAME_PROCESS, GetLastError());
                }
                state = GAME_PROCESS_STATE::GPS_RUNNING; // 状态修改为正在运行
            }
            else
            {
                state = GAME_PROCESS_STATE::GPS_EXITED; // 游戏进程处于退出状态
            }
        }
    }

    GameProcessDetector::GameProcessDetector(std::wstring&& process_name, const std::wstring& launch_cmd):
        Module("游戏进程状态探测器"),
        m_hGameProcess(nullptr, &CloseHandle)
    {
        m_game_process_name = process_name;
        auto l = launch_cmd.length();
        m_LaunchCmd = std::make_unique<wchar_t[]>(l + 1);
        wcscpy_s(m_LaunchCmd.get(), l + 1, launch_cmd.c_str());
    }
}
