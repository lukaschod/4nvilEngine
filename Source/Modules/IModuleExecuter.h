#pragma once

#include <Common\EngineCommon.h>
#include <Modules\Module.h>
#include <Modules\IModulePlanner.h>
#include <vector>

class IModuleExecuter
{
public:
	virtual void Reset() = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual bool IsRunning() = 0;
};