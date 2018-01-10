#pragma once

#include <Tools\Common.h>
#include <Tools\Threading\AutoResetEvent.h>
#include <Tools\Collections\List.h>
#include <Modules\IModuleExecuter.h>
#include <Modules\IModulePlanner.h>
#include <Modules\IProfiler.h>

class Module;

class ModuleManager
{
public:
	ModuleManager(IModulePlanner* planner, IModuleExecuter* executer, IProfiler* profiler);
	~ModuleManager();

	void Start();
	void Stop();
	void NewFrame();
	void WaitForFrame();
	void AddModule(Module* module);
	bool IsRunning() { return executer->IsRunning(); }

	template<class T>
	T* GetModule()
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
	inline uint32_t GetWorkerCount() const { return executer->GetWorkerCount(); }
	inline IProfiler* GetProfiler() const { return profiler; }

private:
	List<Module*> modules;
	IModulePlanner* planner;
	IModuleExecuter* executer;
	AutoResetEvent sleepEvent;
	IProfiler* profiler;
};