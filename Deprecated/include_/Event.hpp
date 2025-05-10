#pragma once
#include <condition_variable>
#include <mutex>

namespace CSOL_Utilities
{
	class Event
	{
	private:
		bool m_signal = false;
		std::mutex m_mtx;
		std::unique_lock<std::mutex> m_lock;
		std::condition_variable m_cond;

	public:
		explicit Event(bool bInitialState = false) : m_signal(bInitialState), m_lock(m_mtx, std::defer_lock) {};
		void set()
		{
			std::lock_guard<std::mutex> lock_guard(m_mtx);
			m_signal = true;
			m_cond.notify_all();
		}
		void reset()
		{
			std::lock_guard<std::mutex> lock_guard(m_mtx);
			m_signal = false;
		}
		/* 在等待前探测 m_Signal 的值，以确定自身是否将会被挂起 */
		bool peek_and_wait()
		{
			bool wait_needed{false};
			std::unique_lock<std::mutex> unique_lock(m_mtx);
			wait_needed = m_signal; /* 记录是否会被阻塞 */
			m_cond.wait(unique_lock, [this] { return m_signal; });
			return wait_needed;
		}
		void wait()
		{
			std::unique_lock<std::mutex> unique_lock(m_mtx);
			m_cond.wait(unique_lock, [this] { return m_signal; });
		}
		bool signaled() const noexcept
		{
			return m_signal;
		}
	};
}; // namespace CSOL_Utilities
