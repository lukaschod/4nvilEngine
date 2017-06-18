#pragma once

#include <Common\EngineCommon.h>
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
	virtual void Recreate(std::vector<Module*>& modules) override;
	virtual void Reset() override;
	virtual ModuleJob TryGetNext() override;
	virtual void SetFinished(ModuleJob module) override;

private:
	StaticModulePlan* plan;
	std::queue<ModuleJob> readyJobs;
	std::mutex readyModulesMutex;
	size_t jobExecutingCount;
};