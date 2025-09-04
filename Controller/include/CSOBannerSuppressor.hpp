#pragma once

#include "Module.hpp"

namespace CSOL_Utilities
{
    class CSOBannerSuppressor : public Module
    {
    public:
        CSOBannerSuppressor(std::wstring cso_banner_path);
        ~CSOBannerSuppressor() noexcept;
        virtual void Resume();
        virtual void Suspend();
    private:
        void Work(std::stop_token st);
        std::mutex m_StateLock;
        std::condition_variable m_RunnableCondition;
        std::condition_variable m_FinishedCondition;
        bool m_bRunnable;
        bool m_bFinished;
        std::thread m_Worker;
        std::wstring m_CSOBannerExecutablePath;
        std::stop_source m_StopSource;
    };
}