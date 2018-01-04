#pragma once

#include <Tools\Common.h>
#include <Modules\IModulePlanner.h>
#include <vector>
#include <queue>
#include <mutex>

class StaticModulePlan;

class StaticModulePlanner : public IModulePlanner
{
public:
	StaticModulePlanner();
	~StaticModulePlanner();
	virtual void Recreate(List<Module*>& modules) override;
	virtual void Reset() override;
	virtual ModuleJob TryGetNext() override;
	virtual void SetFinished(const ModuleJob& module) override;

private:
	StaticModulePlan* plan;
	std::queue<ModuleJob> readyJobs;
	std::mutex readyModulesMutex;
	size_t jobExecutingCount;
};