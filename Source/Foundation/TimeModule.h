#pragma once

#include <Tools\Common.h>
#include <Tools\StopWatch.h>
#include <Modules\Module.h>

class TimeModule : public Module
{
public:
	TimeModule();
	virtual void Execute(const ExecutionContext& context) override;

private:
	StopWatch stopWatch;
	uint64_t passedFrameCount;
};