#include "Signal.hpp"
#include <atomic>
#include <algorithm>
#include <cassert>
#include <memory>
#include <mutex>

namespace CSOL_Utilities
{
    int Signal::peek() noexcept
    {
        auto count = m_count.load(std::memory_order_relaxed);
        assert(count >= 0 && count <= m_SIGNAL_COUNT);
        return count;
    }

    std::shared_ptr<Token> Signal::dispatch(std::unique_lock<std::mutex>& lock)
    {
        assert(lock.owns_lock());
        if (m_requesters.empty()) { return nullptr; }
        m_requesters.erase(
            std::remove_if(
                m_requesters.begin(), m_requesters.end(), [] (std::weak_ptr<Token>& t) {
                    auto tok = t.lock();
                    if (!tok || tok->get_state() != Token::m_SLEEPING) { return true; }
                    return false;
                }),
            m_requesters.end());
        while (!m_requesters.empty())
        {
            auto token_dispatched = m_requesters.front().lock();
            m_requesters.pop_front(); /* 将此令牌移出等待队列 */
            /* 一个令牌允许被多个信号唤醒，需要确认唤醒者是否为自己 */
            if (token_dispatched && token_dispatched->wake(shared_from_this())) /* 尝试为此令牌提供服务 */
            {
                return token_dispatched;
            }
            /* 尝试提供服务失败，说明已经有其他信号抢先为其提供服务，继续尝试为下一个令牌提供服务 */
            else
            {
                continue;
            }
        }
        return nullptr;
    }

    void Signal::release(std::unique_lock<std::mutex>& lock)
    {
        assert(lock.owns_lock());
        auto ret = m_count.fetch_add(1, std::memory_order_release);
        assert(ret <= m_SIGNAL_COUNT);
    }

    void Signal::reclaim()
    {
        std::unique_lock lock(m_token_signal_access_mutex);
        assert(m_count + 1 <= m_SIGNAL_COUNT);
        /* 从等待队列取出一个令牌，为其提供信号 */
        auto token_dispatched = dispatch(lock);
        if (!token_dispatched) /* 没有需要本信号的令牌，将获取到的信号返还 */
        {
            release(lock);
        }
        lock.unlock();
    }

    bool Signal::acquire(std::unique_lock<std::mutex>& lock)
    {
        assert(lock.owns_lock());
        assert(m_count.load(std::memory_order_acquire) >= 0);
        if (m_count == 0)
        {
            return false;
        }
        else
        {
            m_count.fetch_sub(1, std::memory_order_release);
            return true;
        }
    }

    bool Signal::deliver(std::shared_ptr<Token> token)
    {
        if (!token) { return false; }
        std::unique_lock lock(m_token_signal_access_mutex);
        return acquire(lock);
    }

    void Signal::reserve(std::shared_ptr<Token> token)
    {
        std::lock_guard lock(m_token_signal_access_mutex);
        for (auto& tok : m_requesters)
        {
            if (tok.lock() == token)
            {
                return;
            }
        }
        m_requesters.emplace_back(token);
    }

    void UnitToken::acquire()
    {
        if (get_state() == m_ACTIVE)
        {
            return;
        }
        m_state.store(m_AWAKE, std::memory_order_release);
        if (m_signal->deliver(shared_from_this()))
        {
            m_state.store(m_ACTIVE, std::memory_order_seq_cst);
            return;
        }
        m_state.store(m_SLEEPING, std::memory_order_release);
        m_signal->reserve(shared_from_this());
        std::unique_lock lock(m_token_access);
        m_signal_dispatched.wait(lock, [this] { return m_awakened_by; });
        assert(get_state() == m_AWAKE);
        assert(m_awakened_by == m_signal);
        m_awakened_by.reset();
        m_state.store(m_ACTIVE, std::memory_order_release);
    }

    void UnitToken::release()
    {
        if (get_state() != m_ACTIVE)
        {
            return;
        }
        m_state.store(m_INACTIVE, std::memory_order_release);
        m_signal->reclaim();
    }

    bool UnitToken::wake(std::shared_ptr<Signal> signal)
    {
        int sleeping = m_SLEEPING;
        auto awakened_by_this_signal = m_state.compare_exchange_strong(sleeping, m_AWAKE, std::memory_order_seq_cst);
        if (awakened_by_this_signal)
        {
            {
                std::lock_guard lock(m_token_access);
                m_awakened_by = signal;
            }
            m_signal_dispatched.notify_one();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool UnitToken::peek()
    {
        if (get_state() == m_ACTIVE) { return true; }
        return m_signal->peek();
    }
}