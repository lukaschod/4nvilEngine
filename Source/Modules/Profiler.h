#pragma once

#include <Tools\Common.h>
#include <Tools\StopWatch.h>
#include <Modules\IProfiler.h>

class Profiler : public IProfiler
{
public:
	Profiler(uint32_t workersCount);

	virtual void StartFunction(
		uint32_t workerIndex,
		const char* fileName,
		const char* functionName,
		uint32_t lineNumber) override;
	virtual void StopFunction(uint32_t workerIndex) override;
	virtual void Reset() override;
	virtual const List<Worker>& GetWorkers() const { return workers; }

private:
	List<Worker> workers;
	StopWatch stopWatch;
};