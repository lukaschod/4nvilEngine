#pragma once

#include <Tools\Common.h>
#include <Modules\ModuleManager.h>
#include <Tools\Collections\List.h>
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
	Module() {}
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
		module->dependencies.safe_push_back(this);
		return (T*) module;
	}

	template<class T>
	T* ExecuteAfter(ModuleManager* moduleManager)
	{
		ASSERT(moduleManager != nullptr);
		auto module = moduleManager->GetModule<T>();
		dependencies.safe_push_back(module);
		return module;
	}

private:
	AUTOMATED_PROPERTY_GETADR(List<Module*>, dependencies);
};