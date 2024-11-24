#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <deque>

namespace CSOL_Utilities
{
template <typename MailboxType, std::size_t MailboxSize = 10>
class Mailbox
{
private:
    std::condition_variable m_FullCondition;
    std::condition_variable m_EmptyCondition;
    std::mutex m_MailboxMutex;
    std::deque<MailboxType> m_Mailbox;
public:
    Mailbox() = default;
    ~Mailbox() noexcept = default;
    void Post(const MailboxType& m)
    {
        {
            std::unique_lock lock(m_MailboxMutex);
            m_EmptyCondition.wait(lock, [this] () {
                return m_Mailbox.size() < MailboxSize;
            });
            m_Mailbox.push_back(m);
        }
        m_FullCondition.notify_one();
    }
    template <typename Rep, typename Period>
    bool Post(const MailboxType& m, std::chrono::duration<Rep, Period>& duration)
    {
        {
            std::unique_lock lock(m_MailboxMutex);
            auto ret = m_EmptyCondition.wait_for(lock, duration, [this] () {
                return m_Mailbox.size() < MailboxSize;
            });
            if (ret == std::cv_status::timeout) return false;
            m_Mailbox.push_back(m);
        }
        m_FullCondition.notify_one();
        return true;
    }
    void Receive(MailboxType& m)
    {
        {
            std::unique_lock lock(m_MailboxMutex);
            m_FullCondition.wait(lock, [this] () {
                return !m_Mailbox.empty();
            });
            m = m_Mailbox.front();
            m_Mailbox.pop_front();
        }
        m_EmptyCondition.notify_one();
    }
    template <typename Rep, typename Period>
    bool Receive(MailboxType& m, std::chrono::duration<Rep, Period>& duration)
    {
        {
            std::unique_lock lock(m_MailboxMutex);
            auto ret = m_FullCondition.wait_for(lock, duration, [this] () {
                return !m_Mailbox.empty();
            });
            if (ret == std::cv_status::timeout) return false;
            m = m_Mailbox.front();
            m_Mailbox.pop_front();
        }
        m_EmptyCondition.notify_one();
        return true;
    }
};
}