#include <Foundation\TimeModule.h>
#include <Math\Math.h>
#include <stdarg.h>

TimeModule::TimeModule() :
	passedFrameCount(0)
{
	stopWatch.Start();
}

void TimeModule::Execute(const ExecutionContext & context)
{
	stopWatch.Stop();
	if (stopWatch.GetElapsedMiliseconds() >= 1000)
	{
		TRACE("Frame took ms %f", (float) stopWatch.GetElapsedMiliseconds() / passedFrameCount);
		stopWatch.Start();
		passedFrameCount = 0;
	}
	passedFrameCount++;
}
