#pragma once

#include <Common\EngineCommon.h>
#include <vector>

class Module
{
public:
	virtual void Execute(uint32_t offset, size_t size) = 0;
	virtual size_t GetExecutionkSize() = 0;
	virtual size_t GetSplitExecutionTreshold() = 0;

	void AddDependancy(Module* module) { dependencies.push_back(module); }

private:
	AUTOMATED_PROPERTY_GETADR(std::vector<Module*>, dependencies);
};