#include "IdleEngine.hpp"
#include <Windows.h>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <filesystem>
#include <handleapi.h>
#include <string>
#include <tlhelp32.h>
#include <type_traits>
#include <winnt.h>
#include "Global.hpp"
#include "Utilities.hpp"
#include "Console.hpp"
#include "Exception.hpp"

namespace CSOL_Utilities
{
    IdleEngine::IdleEngine(GameProcessInformation game_process_info) :
        m_GameProcessInfo(std::move(game_process_info)),
        m_GameProcessDetector([this] (std::stop_token st) { 
            try {
                DetectGameProcess(st);
            } catch (std::exception& e) {
                Console::Error(e.what());
            }
            Console::Info(Translate("Module::INFO_ModuleExited@1", Translate("IdleEngine::ProcessDetector::NAME")));
            },
            m_StopSource.get_token()),
    
        m_GameStateRecognizer([this] (std::stop_token st) {
            try {
                RecognizeGameState(st);
            } catch (std::exception& e) {
                Console::Error(e.what());
            }
            Console::Info(Translate("Module::INFO_ModuleExited@1", Translate("IdleEngine::OCR::NAME")));
        }, m_StopSource.get_token())
    {
    }

    IdleEngine::~IdleEngine() noexcept
    {
        m_StopSource.request_stop();
        QueueUserAPC([] (ULONG_PTR) {}, reinterpret_cast<HANDLE>(m_GameProcessDetector.native_handle()), 0); /* 检测器可能正在等待进程，需要使用 APC 将其唤醒 */
        m_DetectorRunnable.notify_one();
        m_RecognizerRunnable.notify_one();
        m_GameProcessDetector.join();
        m_GameStateRecognizer.join();
    }

    void IdleEngine::Resume()
    {
        {
            std::lock_guard lk(m_StateLock);
            m_bDetectorRunnable = true;
        }
        m_DetectorRunnable.notify_one();
        m_RecognizerRunnable.notify_one();
    }

    void IdleEngine::Suspend()
    {
        std::unique_lock lk(m_StateLock);
        m_bDetectorRunnable = false;
        if (m_StopSource.get_token().stop_requested())
        {
            return;
        }
        if (m_bDetectorFinished && m_bRecognizerFinished) /* 如果任务都已经执行完毕，则直接返回，减小开销 */
        {
            return;
        }
        QueueUserAPC([] (ULONG_PTR) {}, reinterpret_cast<HANDLE>(m_GameProcessDetector.native_handle()), 0); /* 检测器可能正在等待进程，需要使用 APC 将其唤醒 */
        /* 阻塞等待两个线程完成剩余任务 */
        m_DetectorFinished.wait(lk, [this] { return m_bDetectorFinished; });
        m_RecognizerFinished.wait(lk, [this] { return m_bRecognizerFinished; });
    }

    void IdleEngine::DetectGameProcess(std::stop_token st)
    {
		thread_local GAME_PROCESS_STATE state = GAME_PROCESS_STATE::GPS_UNKNOWN; // 初始状态未知
		// thread_local DWORD m_dwGameProcessId = 0; // 游戏进程标识符

        auto handle_destructor = [] (HANDLE h) {
            if (h == NULL || h == INVALID_HANDLE_VALUE) {
                return;
            }
            CloseHandle(h);
        };

        std::unique_ptr<std::remove_pointer_t<HANDLE>, void (*)(HANDLE)> hGameProcess(nullptr, handle_destructor);

        std::function<bool (const PROCESSENTRY32W&)> check_process = [this] (const PROCESSENTRY32W& process_entry) {
            thread_local std::wstring executable_path;
			if (!m_GameProcessInfo.GameProcessName.starts_with(process_entry.szExeFile))
			{
				return true;
			}
            auto hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, process_entry.th32ProcessID);
            if (hProcess)
            {
                BOOL bSuccess;
                DWORD dwSize;
                while (true)
                {
                    dwSize = executable_path.capacity();
                    bSuccess = QueryFullProcessImageNameW(hProcess, 0, executable_path.data(), &dwSize);
                    if (!bSuccess)
                    {
                        executable_path.resize(executable_path.capacity() + executable_path.capacity() / 2); /* 扩容为 1.5 倍 */
                    }
                    else
                    {
                        break;
                    }
                }
                executable_path.resize(dwSize); /* 调整为写入的长度 */
                std::filesystem::path p(executable_path);
                bool ret;
				std::error_code ec;
                if (std::filesystem::equivalent(p, m_GameProcessInfo.GameExecutablePath, ec)) /* 可执行文件路径相同 */
                {
                    m_GameProcessInfo.dwGameProcessId.store(process_entry.th32ProcessID, std::memory_order_release);
                    ret = false; /* 找到符合条件的进程，停止枚举 */
                }
                else
                {
                    ret = true;
                }
                CloseHandle(hProcess);
                return ret;
            }
            return true; /* 继续枚举 */
        };

        while (true)
        {
            /* 细化封锁粒度，基于作用域解锁，避免死锁 */
            {
                std::unique_lock lk(m_StateLock);
                m_DetectorRunnable.wait(lk, [this, &st] {
                    return st.stop_requested() || m_bDetectorRunnable;
                });
                if (st.stop_requested()) /* 退出 */
                {
                    break;
                }
                m_bDetectorFinished = false;
            }
            if (state == GAME_PROCESS_STATE::GPS_RUNNING) // 游戏进程正在运行
            {
                DWORD dwStatus = WaitForSingleObjectEx(hGameProcess.get(), INFINITE, TRUE); /* 等待进程退出，或新的 APC 出现 */
                if (dwStatus == WAIT_OBJECT_0) /* 游戏进程退出 */
                {
                    std::unique_lock lk(m_StateLock);
                    m_bRecognizerRunnable = false; /* 停止识别器运行 */
                    m_RecognizerFinished.wait(lk, [this] { return m_bRecognizerFinished; }); /* 等待识别器当前轮次执行完毕 */
                    m_GameProcessInfo.dwGameProcessId.store(0, std::memory_order_release); /* 进程号清零 */
                    state = GAME_PROCESS_STATE::GPS_EXITED;
                }
            }
            else if (state == GAME_PROCESS_STATE::GPS_BEING_CREATED) // 游戏进程正在创建
            {
                auto id = m_GameProcessInfo.dwGameProcessId.load(std::memory_order_acquire);
                assert(id == 0);
                EnumProcesses(check_process);
                id = m_GameProcessInfo.dwGameProcessId.load(std::memory_order_acquire);
                if (id) // 查找到有效的 Id
                {
                    hGameProcess.reset(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, id)); // 通过 Id 打开进程
                    if (hGameProcess)
                    {
                        // todo: message
                        {
                            std::lock_guard lk(m_StateLock);
                            m_bRecognizerRunnable = true; /* 允许识别器运行 */
                        }
                        m_RecognizerRunnable.notify_one(); /* 唤醒识别器线程 */
                        state = GAME_PROCESS_STATE::GPS_RUNNING;
                    }
                    else
                    {
                        throw Exception(Translate("IdleEngine::ProcessDetector::ERROR_OpenProcess@1", GetLastError()));
                    }
                }
                m_GameProcessInfo.dwGameProcessId.store(id);
                if (m_ExtendedMode.load(std::memory_order_acquire) && Global::g_DefaultIdleAfterReconnection)
                {
                    m_ExtendedMode.store(false, std::memory_order_release);
                    Console::Info("IdleEngine::ProcessDetector::INFO_SwitchToDefaultIdleMode");
                }
            }
            else if (state == GAME_PROCESS_STATE::GPS_EXITED) // 游戏进程退出，需要重启
            {
                PROCESS_INFORMATION pi;
				STARTUPINFOW si{.cb = sizeof(STARTUPINFOW)};
            
                BOOL bRet = CreateProcessW(
                    nullptr,
                    m_GameProcessInfo.GameProcessLaunchCommand.data(),
                    nullptr,
                    nullptr,
                    false,
                    NORMAL_PRIORITY_CLASS,
                    nullptr,
                    L"C:\\WINDOWS\\SYSTEM32",
                    &si,
                    &pi
                );
                // pi.hProcess, pi.hThread 是游戏启动器进程的句柄，故直接关闭
                if (bRet)
                {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }
                else
                {
                    Console::Warn(Translate("IdleEngine::ProcessDetector::WARN_CreateProcessW@1", GetLastError()));
                }
                state = GAME_PROCESS_STATE::GPS_BEING_CREATED; // 游戏进程正在创建
            }
            else if (state == GAME_PROCESS_STATE::GPS_UNKNOWN)
            {
                auto id = m_GameProcessInfo.dwGameProcessId.load(std::memory_order_acquire);
                assert(id == 0);
                EnumProcesses(check_process);
                id = m_GameProcessInfo.dwGameProcessId.load(std::memory_order_acquire);
                if (id)
                {
                    Console::Info(Translate("IdleEngine::ProcessDetector::INFO_ProcessDetected@1", id));
                    hGameProcess.reset(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE | SYNCHRONIZE,
                                                    FALSE, id)); // 通过 Id 打开进程
                    if (!hGameProcess)
                    {
                        throw Exception(Translate("IdleEngine::ProcessDetector::ERROR_OpenProcess@1", GetLastError()));
                    }
                    m_GameProcessInfo.dwGameProcessId.store(id);
                    state = GAME_PROCESS_STATE::GPS_RUNNING; // 状态修改为正在运行
                    {
                        std::lock_guard lk(m_StateLock);
                        m_bRecognizerRunnable = true;
                    }
                    m_RecognizerRunnable.notify_one();
                }
                else
                {
                    state = GAME_PROCESS_STATE::GPS_EXITED; // 游戏进程处于退出状态
                }
            }
            {
                std::lock_guard lk(m_StateLock);
                m_bDetectorFinished = true;
            }
            m_DetectorFinished.notify_one();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}
