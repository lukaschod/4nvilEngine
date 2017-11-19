#pragma once

#include <Tools\Common.h>
#include <Modules\IModulePlanner.h>
#include <vector>

class Module;

class IModuleExecuter
{
public:
	virtual void Reset() = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual bool IsRunning() = 0;
};