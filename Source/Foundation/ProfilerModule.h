#pragma once

#include <Tools\Common.h>
#include <Modules\PipeModule.h>

class LogModule;

class ProfilerModule : public PipeModule
{
public:
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;

private:
	LogModule* logModule;
};