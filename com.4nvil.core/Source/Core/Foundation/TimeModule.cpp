#include <Core\Tools\Math\Math.hpp>
#include <Core\Foundation\TimeModule.hpp>

using namespace Core;

TimeModule::TimeModule() {}

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