#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <utility>

namespace CSOL_Utilities
{
	class LogicElement;

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
		std::deque<std::weak_ptr<LogicElement>> m_requesters;

	public:
		static auto create(int signal_count = 1)
		{
			return std::make_shared<Signal>(Private{signal_count});
		}
		Signal(Private init) : m_SIGNAL_COUNT(init.signal_count), m_count(init.signal_count)
		{
		}
		static uintptr_t get_current_thread_identifier() noexcept;
		std::shared_ptr<LogicElement> dispatch(std::unique_lock<std::mutex>& lock);
		void reserve(std::shared_ptr<LogicElement> token);
		bool deliver(std::shared_ptr<LogicElement> token);
		void reclaim();
		int peek() noexcept;

	private:
		bool acquire(std::unique_lock<std::mutex>& lock);
		void release(std::unique_lock<std::mutex>& lock);
	};

	class LogicElementFactory;

	class LogicElement : public std::enable_shared_from_this<LogicElement>
	{
		friend class LogicElementFactory;

	protected:
		struct Private
		{
			explicit Private() = default;
		};

	public:
		static const int m_INACTIVE = 0;
		static const int m_SLEEPING = 1;
		static const int m_AWAKE = 2;
		static const int m_ACTIVE = 3;
		LogicElement(Private _)
		{
		}
		/* AndToken 只可能被一个 signal 唤醒；OrToken 会被任意一个 signal 唤醒。 */
		virtual bool wake(std::shared_ptr<Signal> signal) = 0;
		virtual void acquire() = 0;
		virtual bool peek() = 0;
		virtual void release() = 0;
		int get_state() const noexcept
		{
			return m_state.load(std::memory_order_acquire);
		}
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

	class Pin : public LogicElement
	{
	public:
		Pin(Private _, std::shared_ptr<Signal> signal) : LogicElement(_), m_signal(signal)
		{
		}
		static std::shared_ptr<Pin> create(std::shared_ptr<Signal> signal)
		{
			Private _;
			return std::make_shared<Pin>(_, signal);
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
	class AndGate : public LogicElement
	{
	public:
		template <typename... Arguments>
		static std::shared_ptr<AndGate<N>> create(Arguments&&... args)
		{
			Private _;
			auto instance_ref = std::make_shared<AndGate<N>>(_, std::forward<Arguments>(args)...);
			return instance_ref;
		}
		template <typename... Arguments>
		AndGate(Private _, Arguments&&... args) : LogicElement(_), m_signals{std::forward<Arguments>(args)...}
		{
		}
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
	class OrGate : public LogicElement
	{
	public:
		template <typename... Arguments>
		static std::shared_ptr<OrGate<N>> create(Arguments&&... args)
		{
			Private _;
			auto instance_ref = std::make_shared<OrGate<N>>(_, std::forward<Arguments>(args)...);
			return instance_ref;
		}
		template <typename... Arguments>
		OrGate(Private _, Arguments&&... args) : LogicElement(_), m_signals{std::forward<Arguments>(args)...}
		{
		}
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
	void AndGate<N>::acquire()
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
				m_signal_dispatched.wait(lock, [this] { return m_awakened_by; });
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
	void AndGate<N>::release()
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
	bool AndGate<N>::wake(std::shared_ptr<Signal> signal)
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
	bool AndGate<N>::peek()
	{
		if (get_state())
		{
			return true;
		}
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
	void OrGate<N>::acquire()
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
		m_signal_dispatched.wait(lock, [this] { return m_awakened_by; });
		assert(get_state() == m_AWAKE);
		assert(
			[this]
			{
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
	bool OrGate<N>::wake(std::shared_ptr<Signal> signal)
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
	bool OrGate<N>::peek()
	{
		if (get_state())
		{
			return true;
		}
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
	void OrGate<N>::release()
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
		PIN,
		AND_GATE,
		OR_GATE
	};

	class LogicElementFactory
	{
	public:
		template <typename... Arguments>
		static auto make_token(TOKEN_TYPE type, Arguments&&... args)
		{
			switch (type)
			{
			case TOKEN_TYPE::PIN:
				return MakePin(std::forward<Arguments>(args)...);
			case TOKEN_TYPE::AND_GATE:
				return MakeAndGate(std::forward<Arguments>(args)...);
			case TOKEN_TYPE::OR_GATE:
				return MakeOrGate(std::forward<Arguments>(args)...);
			}
		}
		static auto MakePin(std::shared_ptr<Signal> signal) -> std::shared_ptr<Pin>
		{
			return Pin::create(signal);
		}
		template <typename... Arguments>
		static auto MakeAndGate(Arguments&&... args)
		{
			return AndGate<sizeof...(args)>::create(std::forward<Arguments>(args)...);
		}
		template <typename... Arguments>
		static auto MakeOrGate(Arguments&&... args)
		{
			return OrGate<sizeof...(args)>::create(std::forward<Arguments>(args)...);
		}
	};
} // namespace CSOL_Utilities
