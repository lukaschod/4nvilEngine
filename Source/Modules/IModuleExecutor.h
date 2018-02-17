#pragma once

#include <Tools\Common.h>
#include <vector>

class Module;

class IModuleExecutor
{
public:
	// Prepares for new frame
	virtual void Reset() = 0;

	// Initializes the executor
	virtual void Start() = 0;

	// Finalizes the executor
	virtual void Stop() = 0;

	// Returns if the executor is still running
	virtual bool IsRunning() = 0;

	// Returs the number of total workers
	virtual uint32_t GetWorkerCount() = 0;
};