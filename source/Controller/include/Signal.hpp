#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <memory>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <utility>

namespace CSOL_Utilities
{
    class Token;

    class Signal : public std::enable_shared_from_this<Signal>
    {
    private:
        struct Private
        {
            int signal_count = 1;
        };
        std::mutex m_token_signal_access_mutex; /* 访问权限 */
        std::condition_variable m_available; /* 有可用的信号 */
        std::atomic_int m_count = 1; /* 可用的信号数量，-1 表示有线程正在处理此数量 */
        const int m_SIGNAL_COUNT; /* 信号总数 */
        std::mutex m_request_queue_lock;
        std::deque<std::weak_ptr<Token>> m_requesters;

    public:
        static auto create(int signal_count = 1)
        {
            return std::make_shared<Signal>(Private{signal_count});
        }
        Signal(Private init) : m_SIGNAL_COUNT(init.signal_count), m_count(init.signal_count) { }
        static uintptr_t get_current_thread_identifier() noexcept;
        std::shared_ptr<Token> dispatch(std::unique_lock<std::mutex>& lock);
        void reserve(std::shared_ptr<Token> token);
        bool deliver(std::shared_ptr<Token> token);
        void reclaim();
        int peek() noexcept;
    private:
        bool acquire(std::unique_lock<std::mutex>& lock);
        void release(std::unique_lock<std::mutex>& lock);
    };

    class TokenFactory;

    class Token : public std::enable_shared_from_this<Token>
    {
        friend class TokenFactory;
    protected:
        struct Private { explicit Private() = default; };
    public:
        static const int m_INACTIVE = 0;
        static const int m_SLEEPING = 1;
        static const int m_AWAKE = 2;
        static const int m_ACTIVE = 3;
        Token(Private _) { }
        /* AndToken 只可能被一个 signal 唤醒；OrToken 会被任意一个 signal 唤醒。 */
        virtual bool wake(std::shared_ptr<Signal> signal) = 0;
        virtual void acquire() = 0;
        virtual bool peek() = 0;
        virtual void release() = 0;
        int get_state() const noexcept{ return m_state.load(std::memory_order_acquire); }
        bool possess() noexcept
        {
            bool not_possessed_yet = false; /* 希望尚未被占据 */
            return m_possessed.compare_exchange_strong(not_possessed_yet, true, std::memory_order_seq_cst);
        }
    protected:
        std::atomic_bool m_possessed = false;
        std::mutex m_token_access;
        std::condition_variable m_signal_dispatched;
        std::atomic_int m_state = m_INACTIVE;
        std::shared_ptr<Signal> m_awakened_by;
    };

    class UnitToken : public Token
    {
    public:
        UnitToken(Private _, std::shared_ptr<Signal> signal) :
            Token(_),
            m_signal(signal)
        {}
        static std::shared_ptr<UnitToken> create(std::shared_ptr<Signal> signal)
        {
            Private _;
            return std::make_shared<UnitToken>(_, signal);
        }
        virtual bool wake(std::shared_ptr<Signal> signal) override;
        virtual void acquire() override;
        virtual bool peek() override;
        virtual void release() override;
    private:
        std::shared_ptr<Signal> m_signal;
    };

    template <int N>
    concept signal_count_restriction = N > 0 && N < 114514;

    template <int N>
    requires signal_count_restriction<N>
    class AndToken : public Token
    {
    public:
        template <typename... Arguments>
        static std::shared_ptr<AndToken<N>> create(Arguments&&... args)
        {
            Private _;
            auto instance_ref = std::make_shared<AndToken<N>>(_, std::forward<Arguments>(args)...);
            return instance_ref;
        }
        template <typename... Arguments>
        AndToken(Private _, Arguments&&... args) :
            Token(_),
            m_signals{std::forward<Arguments>(args)...}
        { }
        virtual bool wake(std::shared_ptr<Signal> signal) override;
        virtual void acquire() override;
        virtual bool peek() override;
        virtual void release() override;
    protected:
        std::array<std::shared_ptr<Signal>, N> m_signals;
        int m_checkpoint = -1;
    };

    template <int N>
    requires signal_count_restriction<N>
    class OrToken : public Token
    {
    public:
        template <typename... Arguments>
        static std::shared_ptr<OrToken<N>> create(Arguments&&... args)
        {
            Private _;
            auto instance_ref = std::make_shared<OrToken<N>>(_, std::forward<Arguments>(args)...);
            return instance_ref;
        }
        template <typename... Arguments>
        OrToken(Private _, Arguments&&... args) :
            Token(_),
            m_signals{std::forward<Arguments>(args)...}
        { }
        virtual bool wake(std::shared_ptr<Signal> signal) override;
        virtual void acquire() override;
        virtual bool peek() override;
        virtual void release() override;

    protected:
        std::array<std::shared_ptr<Signal>, N> m_signals;
        std::shared_ptr<Signal> m_acquired_signal;
    };

    template <int N>
    requires signal_count_restriction<N>
    void AndToken<N>::acquire()
    {
        if (get_state() == m_ACTIVE)
        {
            return;
        }
        int checkpoint = -1; /* 当需要阻塞等待时存点 */
        m_state.store(m_AWAKE, std::memory_order_release); /* 等待 */
        for (int i = 0; i < N; i++)
        {
            /* 存点处的信号已经获取，跳过 */
            if (i == checkpoint)
            {
                continue;
            }
            /* 尝试立即获取信号 */
            if (!m_signals[i]->deliver(shared_from_this()))
            {
                /* 无法立即获取当前信号，逆序释放先前获取的所有信号 */
                for (int j = i - 1; j >= 0; j--)
                {
                    if (j != checkpoint)
                    {
                        m_signals[j]->reclaim();
                    }
                }
                /* 存点处的信号最后释放 */
                if (checkpoint >= 0)
                {
                    m_signals[checkpoint]->reclaim();
                }
                m_state.store(m_SLEEPING, std::memory_order_release); /* 等待 */
                m_signals[i]->reserve(shared_from_this()); /* 在此信号处阻塞 */
                std::unique_lock lock(m_token_access);
                m_signal_dispatched.wait(lock, [this] {
                    return m_awakened_by;
                });
                assert(m_awakened_by == m_signals[i]);
                assert(get_state() == m_AWAKE);
                checkpoint = i; /* 存点 */
                i = -1; /* 从头开始依次尝试 */
                m_awakened_by.reset();
                continue;
            }
            else
            {
                m_state.store(m_AWAKE, std::memory_order_release);
                /* 成功获取当前信号 */
            }
        }
        m_checkpoint = checkpoint;
        m_state.store(m_ACTIVE, std::memory_order_release);
    }

    template <int N>
    requires signal_count_restriction<N>
    void AndToken<N>::release()
    {
        if (get_state() != m_ACTIVE)
        {
            return;
        }
        m_state.store(m_INACTIVE, std::memory_order_seq_cst);
        for (int i = N - 1; i >= 0; i--)
        {
            if (i != m_checkpoint)
            {
                m_signals[i]->reclaim();
            }
            else
            {
            }
        }
        if (m_checkpoint >= 0)
        {
            m_signals[m_checkpoint]->reclaim();
            m_checkpoint = -1;
        }
    }

    template <int N>
    requires signal_count_restriction<N>
    bool AndToken<N>::wake(std::shared_ptr<Signal> signal)
    {
        int sleeping = m_SLEEPING; /* 希望其他信号尚未唤醒此令牌 */
        auto awakened_by_this_signal = m_state.compare_exchange_strong(sleeping, m_AWAKE, std::memory_order_seq_cst);
        if (awakened_by_this_signal) /* 成功唤醒 */
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

    template <int N>
    requires signal_count_restriction<N>
    bool AndToken<N>::peek()
    {
        if (get_state()) { return true; }
        for (auto sig : m_signals)
        {
            if (!sig->peek())
            {
                return false;
            }
        }
        return true;
    }

    template <int N>
    requires signal_count_restriction<N>
    void OrToken<N>::acquire()
    {
        if (get_state() == m_ACTIVE)
        {
            assert(m_acquired_signal);
            return;
        }

        m_state.store(m_AWAKE, std::memory_order_release);
        for (auto& s : m_signals)
        {
            if (s->deliver(shared_from_this()))
            {
                m_acquired_signal = s;
                m_state.store(m_ACTIVE, std::memory_order_release);
                return;
            }
        }

        m_state.store(m_SLEEPING, std::memory_order_release);
        for (auto& s : m_signals)
        {
            s->reserve(shared_from_this()); /* 预定信号 */
        }
        std::unique_lock lock(m_token_access);
        m_signal_dispatched.wait(lock, [this] {
            return m_awakened_by;
        });
        assert(get_state() == m_AWAKE);
        assert([this] {
            for (auto& s : m_signals)
            {
                if (s == m_awakened_by)
                {
                    return true;
                }
            }
            return false;
        }());
        std::swap(m_awakened_by, m_acquired_signal);
        m_state.store(m_ACTIVE, std::memory_order_release);
    }

    template <int N>
    requires signal_count_restriction<N>
    bool OrToken<N>::wake(std::shared_ptr<Signal> signal)
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

    template <int N>
    requires signal_count_restriction<N>
    bool OrToken<N>::peek()
    {
        if (get_state()) { return true; }
        for (auto& s : m_signals)
        {
            if (s->peek())
            {
                return true;
            }
        }
        return false;
    }

    template <int N>
    requires signal_count_restriction<N>
    void OrToken<N>::release()
    {
        if (get_state() != m_ACTIVE)
        {
            return;
        }
        assert(m_acquired_signal);
        m_state.store(m_INACTIVE, std::memory_order_release);
        m_acquired_signal->reclaim();
        m_acquired_signal.reset();
    }


    enum class TOKEN_TYPE
    {
        UNIT_TOKEN,
        AND_TOKEN,
        OR_TOKEN
    };

    class TokenFactory
    {
    public:
        template <typename... Arguments>
        static auto make_token(TOKEN_TYPE type, Arguments&&... args)
        {
            switch (type)
            {
            case TOKEN_TYPE::UNIT_TOKEN: return make_unit_token(std::forward<Arguments>(args)...);
            case TOKEN_TYPE::AND_TOKEN: return make_and_token(std::forward<Arguments>(args)...);
            case TOKEN_TYPE::OR_TOKEN: return make_or_token(std::forward<Arguments>(args)...);
            }
        }
        static auto make_unit_token(std::shared_ptr<Signal> signal) -> std::shared_ptr<UnitToken>
        {
            return UnitToken::create(signal);
        }
        template <typename... Arguments>
        static auto make_and_token(Arguments&&... args)
        {
            return AndToken<sizeof...(args)>::create(std::forward<Arguments>(args)...);
        }
        template <typename... Arguments>
        static auto make_or_token(Arguments&&... args)
        {
            return OrToken<sizeof...(args)>::create(std::forward<Arguments>(args)...);
        }
    };
}
