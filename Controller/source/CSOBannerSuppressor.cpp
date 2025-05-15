#include "pch.hpp"

#include "Console.hpp"
#include "CSOBannerSuppressor.hpp"
#include "Console.hpp"
#include "Utilities.hpp"

namespace CSOL_Utilities
{
    CSOBannerSuppressor::CSOBannerSuppressor(std::wstring cso_banner_executable_path) :
        m_CSOBannerExecutablePath(std::move(cso_banner_executable_path))
    {
        m_Worker = std::thread(
            [this] {
				std::string module_name = "CSOBannerSuppressor";
                try{
                    Work(m_StopSource.get_token());
                } catch (std::exception& e) {
                    Console::Error(Translate("Module::ERROR_ModulePanic@2", module_name, e.what()));
                }
                Console::Info(Translate("Module::INFO_ModuleExited@1", module_name));
            }
        );
    }

    CSOBannerSuppressor::~CSOBannerSuppressor() noexcept
    {
        m_StopSource.request_stop();
        m_RunnableCondition.notify_one();
        QueueUserAPC([] (ULONG_PTR) {}, reinterpret_cast<HANDLE>(m_Worker.native_handle()), 0);
        m_Worker.join();
    }

    void CSOBannerSuppressor::Suspend()
    {
        std::unique_lock lk(m_StateLock);
        m_bRunnable = false;
        if (m_bFinished)
        {
            return;
        }
        m_FinishedCondition.wait(lk, [this] { return m_bFinished; });
    }

    void CSOBannerSuppressor::Resume()
    {
        {
            std::lock_guard lk(m_StateLock);
            m_bRunnable = true;
        }
        m_RunnableCondition.notify_one();
    }

    void CSOBannerSuppressor::Work(std::stop_token st)
    {
        const std::wstring cso_banner_executable_name = L"CSOBanner.exe";
        auto find_cso_banner_process = [this, &cso_banner_executable_name] (const PROCESSENTRY32W& process_entry)  -> bool {
            if (cso_banner_executable_name != process_entry.szExeFile)
            {
                return true; /* 继续遍历 */
            }
            thread_local std::wstring executable_path;
            auto hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, false, process_entry.th32ProcessID);
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
                        executable_path.resize(executable_path.capacity() + executable_path.capacity() / 2);
                    }
                    else
                    {
                        break;
                    }
                }
                executable_path.resize(dwSize);
                std::filesystem::path p(executable_path);
                bool ret;
                std::error_code ec;
                if (std::filesystem::equivalent(p, m_CSOBannerExecutablePath, ec))
                {
                    SafeTerminateProcess(hProcess, 1500); /* 安全地结束进程 */
                    CloseHandle(hProcess);
                    Console::Info(Translate("CSOBannerSuppressor::INFO_CSOBannerSuppressed"));
                }
            }
			return false; /* CSOBanner 允许多个实例运行，故遍历所有进程 */
        };
        while (true)
        {
            {
                std::unique_lock lk(m_StateLock);
                m_RunnableCondition.wait(lk, [this, &st] {
                    return st.stop_requested() || m_bRunnable;
                });
                if (st.stop_requested())
                {
                    break;
                }
                m_bFinished = false;
            }
            {
                std::lock_guard lk(m_StateLock);
                EnumerateProcesses(find_cso_banner_process);
                m_bFinished = true;
            }
            m_FinishedCondition.notify_one();
            SleepEx(5000, true);
        }
    }
}
