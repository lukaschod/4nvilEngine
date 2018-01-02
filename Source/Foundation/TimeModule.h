#pragma once

#include <Tools\Common.h>
#include <Tools\StopWatch.h>
#include <Modules\Module.h>

class LogModule;

class TimeModule : public Module
{
public:
	TimeModule();
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;

private:
	LogModule* logModule;
	StopWatch stopWatch;
	uint64_t passedFrameCount;
};