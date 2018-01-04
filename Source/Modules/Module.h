#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <Modules\ModuleManager.h>
#include <typeinfo>

class IGraphicsModule;
class Module;

struct ExecutionContext
{
	uint32_t workerIndex;
	uint32_t offset;
	size_t size;
	Module* executingModule;
};

class Module
{
public:
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) {}
	virtual void Execute(const ExecutionContext& context) = 0;
	virtual size_t GetExecutionSize() { return 1; }
	virtual size_t GetSplitExecutionSize(size_t currentSize) { return 1; }
	virtual const char* GetName() { return "Unamed";  }

protected:
	template<class T>
	T* ExecuteBefore(ModuleManager* moduleManager)
	{
		ASSERT(moduleManager != nullptr);
		auto module = (Module*) moduleManager->GetModule<T>();
		if (module->dependencies.safe_push_back(this))
			module->OnDependancyAdd(moduleManager, this, true);
		return (T*) module;
	}

	template<class T>
	T* ExecuteAfter(ModuleManager* moduleManager)
	{
		ASSERT(moduleManager != nullptr);
		auto module = (Module*) moduleManager->GetModule<T>();
		if (dependencies.safe_push_back(module))
			module->OnDependancyAdd(moduleManager, this, false);
		return (T*) module;
	}

	virtual void OnDependancyAdd(ModuleManager* moduleManager, Module* module, bool executeBefore) {}

private:
	AUTOMATED_PROPERTY_GETADR(List<Module*>, dependencies);
};