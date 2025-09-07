#include "Console.hpp"
#include "CSOBannerSuppressor.hpp"
#include "Console.hpp"
#include "Utilities.hpp"

namespace CSOL_Utilities
{
    CSOBannerSuppressor::CSOBannerSuppressor(std::wstring cso_banner_executable_path) :
        CSO_Banner_executable_path_(std::move(cso_banner_executable_path))
    {
    }

    CSOBannerSuppressor::~CSOBannerSuppressor() noexcept
    {
        Terminate();
    }

    void CSOBannerSuppressor::Boot()
    {
        std::lock_guard lk(boot_lock_);
        if (is_booted_) return; // 防止重复启动
        worker_ = std::thread(
            [this] {
				std::string module_name = "CSOBannerSuppressor";
                try{
                    Run(stop_source_.get_token());
                } catch (std::exception& e) {
                    Console::Error(Translate("Module::ERROR_ModulePanic@2", module_name, e.what()));
                }
                Console::Info(Translate("Module::INFO_ModuleExited@1", module_name));
            }
        );
        is_booted_ = true; // 标记为启动
    }

    void CSOBannerSuppressor::Resume() noexcept
    {
        {
            std::lock_guard lk(worker_state_lock_);
            is_worker_runnable_ = true;
        }
        worker_runnable_cond_.notify_one();
    }

    void CSOBannerSuppressor::Suspend() noexcept
    {
        std::unique_lock lk(worker_state_lock_);
        is_worker_runnable_ = false;
        if (has_worker_finished_)
        {
            return;
        }
        worker_finished_cond_.wait(lk, [this] { return has_worker_finished_; });
    }

    void CSOBannerSuppressor::Terminate() noexcept
    {
        std::lock_guard lk(boot_lock_);
        if (!is_booted_) return; // 未启动
        stop_source_.request_stop();
        worker_runnable_cond_.notify_one();
        QueueUserAPC([] (ULONG_PTR) {}, reinterpret_cast<HANDLE>(worker_.native_handle()), 0);
        if (worker_.joinable())
            worker_.join();
        is_booted_ = false; // 标记为未启动
    }

    void CSOBannerSuppressor::Run(std::stop_token st)
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
                if (std::filesystem::equivalent(p, CSO_Banner_executable_path_, ec))
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
                std::unique_lock lk(worker_state_lock_);
                worker_runnable_cond_.wait(lk, [this, &st] {
                    return st.stop_requested() || is_worker_runnable_;
                });
                if (st.stop_requested())
                {
                    break;
                }
                has_worker_finished_ = false;
            }
            {
                std::lock_guard lk(worker_state_lock_);
                EnumerateProcesses(find_cso_banner_process);
                has_worker_finished_ = true;
            }
            worker_finished_cond_.notify_one();
            SleepEx(5000, true);
        }
    }
}
