#pragma once

#include <Tools\Common.h>
#include <Tools\Threading\AutoResetEvent.h>
#include <Modules\IModuleExecuter.h>
#include <Modules\IModulePlanner.h>
#include <Tools\Collections\List.h>

class Module;

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

private:
	List<Module*> modules;
	IModulePlanner* planner;
	IModuleExecuter* executer;
	AutoResetEvent sleepEvent;
};