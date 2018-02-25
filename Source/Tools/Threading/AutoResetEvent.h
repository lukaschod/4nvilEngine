#pragma once

#include <mutex>
#include <condition_variable>

namespace Core::Threading
{
	class AutoResetEvent
	{
	public:
		explicit AutoResetEvent(bool initial = false);

		void Set();
		void Reset();
		bool WaitOne();

	private:
		AutoResetEvent(const AutoResetEvent&);
		AutoResetEvent& operator=(const AutoResetEvent&); // non-copyable

	private:
		std::condition_variable signal;
		std::mutex protect;
		bool flag;
	};
}