#pragma once

#include "Utilities.hpp"
#include "Exception.hpp"

namespace CSOL_Utilities
{
	template <std::size_t N>
	concept WAIT_EVENTS_MAX = N <= 0x80;

	class Event : public std::enable_shared_from_this<Event>
	{
		template <std::size_t N>
		friend void WaitForMultipleEvents(std::shared_ptr<Event> (&events)[N], bool wait_all);
		template <std::size_t N, typename Rep, typename Period>
		friend bool WaitForMultipleEvents(std::shared_ptr<Event> (&events)[N], bool wait_all, std::chrono::duration<Rep, Period> duration);
		struct Init
		{
			bool bAutoReset;
			bool bInitialState;
		};
	public:
		Event(Init& init)
		{
			m_hEvent = CreateEventW(nullptr, !init.bAutoReset, init.bInitialState, L"");
			if (!m_hEvent)
			{
				throw Exception(Translate("Event::ERROR_CreateEventW@1", GetLastError()));
			}
		}
		static std::shared_ptr<Event> New(bool auto_reset, bool initial_state)
		{
			Init init(auto_reset, initial_state);
			return std::make_shared<Event>(init);
		}
		void Wait() const
		{
			WaitForSingleObject(m_hEvent, INFINITE);
		}
		template <typename Rep, typename Period>
		bool Wait(std::chrono::duration<Rep, Period> duration) const
		{
			auto infinite = std::chrono::milliseconds(INFINITE);
			if (duration > infinite)
			{
				return WaitForSingleObject(m_hEvent, INFINITE) == WAIT_OBJECT_0;
			}
			else
			{
				return WaitForSingleObject(m_hEvent, duration.count()) == WAIT_OBJECT_0;
			}
		}
		void Set() const
		{
			SetEvent(m_hEvent);
		}
		void Reset() const
		{
			ResetEvent(m_hEvent);
		}
		bool Peek() const
		{
			auto ret = WaitForSingleObject(m_hEvent, 0);
			if (ret == WAIT_OBJECT_0)
			{
				return true;
			}
			return false;
		}
		~Event() noexcept
		{
			CloseHandle(m_hEvent);
		}
	private:
		HANDLE m_hEvent;
	};

	template <std::size_t N>
	requires WAIT_EVENTS_MAX<N>
	void WaitForMultipleEvents(std::shared_ptr<Event> (&events)[N], bool wait_all)
	{
		HANDLE hEvent[N];
		for (std::size_t i = 0; i < N; i++)
		{
			hEvent[i] = events[i]->m_hEvent;
		}
		WaitForMultipleObjects(N, hEvent, wait_all, INFINITE);
	}

	template <std::size_t N, typename Rep, typename Period>
	requires WAIT_EVENTS_MAX<N>
	bool WaitForMultipleEvents(std::shared_ptr<Event> (&events)[N], bool wait_all, std::chrono::duration<Rep, Period> duration)
	{
		HANDLE hEvent[N];
		for (std::size_t i = 0; i < N; i++)
		{
			hEvent[i] = events[i]->m_hEvent;
		}
		auto infinite = std::chrono::milliseconds(INFINITE);
		if (duration > infinite)
		{
			return WaitForMultipleObjects(N, hEvent, wait_all, INFINITE) != WAIT_TIMEOUT;
		}
		else
		{
			return WaitForMultipleObjects(N, hEvent, wait_all, duration.count()) != WAIT_TIMEOUT;
		}
	}

}; // namespace CSOL_Utilities
