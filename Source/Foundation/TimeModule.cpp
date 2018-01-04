#include <Foundation\TimeModule.h>
#include <Foundation\LogModule.h>
#include <Tools\Math\Math.h>

TimeModule::TimeModule() 
	: passedFrameCount(0)
{
	stopWatch.Start();
}

void TimeModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	logModule = ExecuteBefore<LogModule>(moduleManager);
}

void TimeModule::Execute(const ExecutionContext& context)
{
	stopWatch.Stop();
	if (stopWatch.GetElapsedMiliseconds() >= 8000)
	{
		// 78 58 59
		// 69 59 61
		// 67 57 57
		// 36 33 33
		TRACE("Frame took ms %f", (float) stopWatch.GetElapsedMiliseconds() / passedFrameCount);
		logModule->RecWriteFmt(context, "Frame took ms %f\n", (float) stopWatch.GetElapsedMiliseconds() / passedFrameCount);
		stopWatch.Start();
		passedFrameCount = 0;
	}
	passedFrameCount++;
}
