#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>

#define PROFILER_ENABLED

#ifdef PROFILER_ENABLED
#	define PROFILE_FUNCTION ProfileFunction profileFunction(context.workerIndex, profiler, __FILE__, __FUNCTION__, __LINE__)
#else
#	define PROFILE_FUNCTION (void) 0
#endif

class IProfiler
{
public:
	struct Function;

	struct Function
	{
		const char* functionName;
		const char* fileName;
		uint32_t lineNumber;
		uint64_t startCycle;
		uint64_t endCycle;
		Function* parent;
	};

	struct Worker
	{
		List<Function> functions;
		Function* currentFunction;
	};

public:
	virtual void StartFunction(
		uint32_t workerIndex,
		const char* fileName, 
		const char* functionName, 
		uint32_t lineNumber) = 0;
	virtual void StopFunction(uint32_t workerIndex) = 0;
	virtual void Reset() = 0;
	virtual const List<Worker>& GetWorkers() const = 0;
};

struct ProfileFunction
{
public:
	ProfileFunction(
		uint32_t workerIndex,
		IProfiler* profiler,
		const char* fileName,
		const char* functionName,
		uint32_t lineNumber)
		: profiler(profiler)
		, workerIndex(workerIndex)
	{
		profiler->StartFunction(workerIndex, fileName, functionName, lineNumber);
	}

	~ProfileFunction()
	{
		profiler->StopFunction(workerIndex);
	}

private:
	IProfiler* profiler;
	uint32_t workerIndex;
};