#include <Foundation\ProfilerModule.h>
#include <Foundation\LogModule.h>
#include <Tools\Math\Math.h>

void ProfilerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	Module::SetupExecuteOrder(moduleManager);
	logModule = ExecuteBefore<LogModule>(moduleManager);
}

void ProfilerModule::Execute(const ExecutionContext& context)
{
	PROFILE_FUNCTION;
}
