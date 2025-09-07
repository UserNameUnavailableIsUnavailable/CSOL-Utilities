#pragma once

#include "Module.hpp"

namespace CSOL_Utilities
{
    class CSOBannerSuppressor : public Module
    {
    public:
        CSOBannerSuppressor(std::wstring cso_banner_path);
        ~CSOBannerSuppressor() noexcept;
        virtual void Boot() override;
        virtual void Resume() noexcept override;
        virtual void Suspend() noexcept override;
        virtual void Terminate() noexcept override;
    private:
        std::mutex boot_lock_;
        bool is_booted_ = false;

        void Run(std::stop_token st);
        std::mutex worker_state_lock_;
        std::condition_variable worker_runnable_cond_;
        std::condition_variable worker_finished_cond_;
        bool is_worker_runnable_;
        bool has_worker_finished_;
        std::thread worker_;
        std::wstring CSO_Banner_executable_path_;
        std::stop_source stop_source_;
    };
}