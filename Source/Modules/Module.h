#pragma once

#include <Common\EngineCommon.h>
#include <vector>
#include <typeinfo>

class GraphicsModule;

class Module
{
public:
	virtual void Execute(uint32_t offset, size_t size) = 0;
	virtual size_t GetExecutionkSize() = 0;
	virtual size_t GetSplitExecutionTreshold() = 0;

	void AddDependancy(Module* module) { dependencies.push_back(module); }

protected:
	template<class T>
	T* GetDependancy()
	{
		FOR_EACH(dependencies, itr)
		{
			auto dependancy = *itr;
			if (dynamic_cast<T*>(dependancy) != 0)
				return (T*)dependancy;
		}
		LogError("Can't find specified dependancy");
		return nullptr;
	}

private:
	AUTOMATED_PROPERTY_GETADR(std::vector<Module*>, dependencies);
};