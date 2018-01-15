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
	inline uint64_t GetDeltaTimeMs() const { return stopWatch.GetElapsedMiliseconds(); }

private:
	StopWatch stopWatch;
	uint64_t deltaTimeMs;
};