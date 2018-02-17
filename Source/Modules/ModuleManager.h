#pragma once

#include <Tools\Common.h>
#include <Tools\Threading\AutoResetEvent.h>
#include <Tools\Collections\List.h>
#include <Modules\IModuleExecutor.h>
#include <Modules\IModulePlanner.h>

class Module;

class ModuleManager
{
public:
	ModuleManager(IModulePlanner* planner, IModuleExecutor* executor);
	~ModuleManager();

	// Initializes the ModuleManager and other Modules
	void Start();

	// Finalizes ModuleManager and other Modules
	void Stop();

	// Begins new iteration of frame, where all modules will be re-executed at given time
	void NewFrame();

	// Waits until the current frame finishes
	void WaitForFrame();

	// Adds module to manager, which later on will be used for planning and execution
	void AddModule(Module* module);

	void RequestStop() { requestedStop = true; }
	bool IsRunning() { return !requestedStop; }

	template<class T> T* GetModule()
	{
		for (auto module : modules)
		{
			if (dynamic_cast<T*>(module) != 0)
				return (T*) module;
		}
		ERROR("Can't find specified module");
		return nullptr;
	}

	inline const List<Module*>& GetModules() const { return modules; }
	inline uint32_t GetWorkerCount() const { return executor->GetWorkerCount(); }

private:
	List<Module*> modules; // Modules contained by the manager
	IModulePlanner* planner; // Planner that will be used for planning Module execution
	IModuleExecutor* executor; // Executor that will be sued for Module execution
	AutoResetEvent sleepEvent;
	bool requestedStop;
};