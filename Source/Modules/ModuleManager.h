#pragma once

#include <Common\EngineCommon.h>
#include <Common\AutoResetEvent.h>
#include <Modules\IModuleExecuter.h>
#include <Modules\IModulePlanner.h>
#include <Modules\Module.h>
#include <vector>

class ModuleManager
{
public:
	ModuleManager(IModulePlanner* planner, IModuleExecuter* executer);
	~ModuleManager();

	void Start();
	void Stop();
	void NewFrame();
	void WaitForFrame();
	void AddModule(Module* module);
	bool IsRunning() { return executer->IsRunning(); }

private:
	std::vector<Module*> modules;
	IModulePlanner* planner;
	IModuleExecuter* executer;
	AutoResetEvent sleepEvent;
};