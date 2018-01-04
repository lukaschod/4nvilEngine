#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <functional>

class Module;

struct ModuleJob
{
	ModuleJob() 
		: module(nullptr)
		, offset(0)
		, size(0)
		, userData(nullptr) 
	{}
	Module* module;
	uint32_t offset;
	size_t size;
	void* userData;
};

class IModulePlanner
{
public:
	virtual void Recreate(List<Module*>& modules) = 0;
	virtual void Reset() = 0;
	virtual ModuleJob TryGetNext() = 0;
	virtual void SetFinished(const ModuleJob& job) = 0;

public:
	AUTOMATED_PROPERTY_SET(std::function<void(size_t)>, jobFinishCallback);
	AUTOMATED_PROPERTY_SET(std::function<void(void)>, finishCallback);
};