#pragma once
#include <condition_variable>
#include <mutex>
namespace CSOL_Utilities
{
class CMannualEvent
{
  private:
    bool m_Signal = false;
    std::mutex m_Mutex;
    std::unique_lock<std::mutex> m_Lock;
    std::condition_variable m_CondVar;

  public:
    explicit CMannualEvent(bool bInitialState = false) : m_Signal(bInitialState), m_Lock(m_Mutex, std::defer_lock) {};
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
    }
    /* 在等待前探测 m_Signal 的值，以确定自身是否将会被挂起 */
    bool PeekAndWait()
    {
        bool wait_needed{false};
        std::unique_lock<std::mutex> unique_lock(m_Mutex);
        wait_needed = m_Signal; /* 记录是否会被阻塞 */
        m_CondVar.wait(unique_lock, [this] { return m_Signal; });
        return wait_needed;
    }
    void Wait()
    {
        std::unique_lock<std::mutex> unique_lock(m_Mutex);
        m_CondVar.wait(unique_lock, [this] { return m_Signal; });
    }
    bool Signaled() const noexcept
    {
        return m_Signal;
    }
};
}; // namespace CSOL_Utilities