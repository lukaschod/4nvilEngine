#include <Foundation\TimeModule.h>
#include <Tools\Math\Math.h>

TimeModule::TimeModule() 
{
}

void TimeModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	stopWatch.Start();
}

void TimeModule::Execute(const ExecutionContext& context)
{
	stopWatch.Stop();
	deltaTimeMs = stopWatch.GetElapsedMiliseconds();
	deltaTime = deltaTimeMs / 1000.0f;
	stopWatch.Start();
}