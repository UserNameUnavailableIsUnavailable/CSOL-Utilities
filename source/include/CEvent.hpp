#pragma once
#include <mutex>
#include <condition_variable>
namespace CSOL_Utilities
{
    class CEvent
    {
    private:
        bool m_Signal = false;
        std::mutex m_Mutex;
        std::condition_variable m_CondVar;
    public:
        explicit CEvent(bool bInitialState = false) : m_Signal(bInitialState) {};
        void Set()
        {
            std::lock_guard<std::mutex> lock_guard(m_Mutex);
            m_Signal = true;
            m_CondVar.notify_all();
        }
        void Reset()
        {
            std::lock_guard<std::mutex> lock_guard(m_Mutex);
            m_Signal = false;
            m_CondVar.notify_all();
        }
        void Wait()
        {
            std::unique_lock<std::mutex> unique_lock(m_Mutex);
            m_CondVar.wait(unique_lock, [this] { return m_Signal; });
        }
        bool Signaled() noexcept
        {
            return m_Signal;
        }
    };
};