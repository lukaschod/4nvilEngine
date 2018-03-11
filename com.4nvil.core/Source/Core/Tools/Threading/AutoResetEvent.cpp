#include <Core\Tools\Threading\AutoResetEvent.h>

using namespace Core::Threading;

AutoResetEvent::AutoResetEvent(bool initial)
	: flag(initial)
{
}

void AutoResetEvent::Set()
{
	std::lock_guard<std::mutex> _(protect);
	flag = true;
	signal.notify_one();
}

void AutoResetEvent::Reset()
{
	std::lock_guard<std::mutex> _(protect);
	flag = false;
}

bool AutoResetEvent::WaitOne()
{
	std::unique_lock<std::mutex> lk(protect);
	while (!flag) // prevent spurious wakeups from doing harm
		signal.wait(lk);
	flag = false; // waiting resets the flag
	return true;
}