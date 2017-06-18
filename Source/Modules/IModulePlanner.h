#pragma once

#include <Common\EngineCommon.h>
#include <Modules\Module.h>
#include <vector>
#include <functional>

struct ModuleJob
{
	ModuleJob() : module(nullptr), offset(0), size(0) {}
	Module* module;
	uint32_t offset;
	size_t size;
};

class IModulePlanner
{
public:
	virtual void Recreate(std::vector<Module*>& modules) = 0;
	virtual void Reset() = 0;
	virtual ModuleJob TryGetNext() = 0;
	virtual void SetFinished(ModuleJob job) = 0;

public:
	AUTOMATED_PROPERTY_SET(std::function<void(uint32_t)>, jobFinishCallback);
	AUTOMATED_PROPERTY_SET(std::function<void(void)>, finishCallback);
};