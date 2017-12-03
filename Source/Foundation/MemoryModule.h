#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\AllocatorBuddyBlock.h>
#include <Modules\Module.h>
#include <map>

class MemoryModule : public Module
{
public:
	MemoryModule();
	virtual void Execute(const ExecutionContext& context) override;

	template<class T, class ... N>
	T* New(uint32_t memoryType, N ... args)
	{
		auto allocator = allocators.find(memoryType);
		if (allocator == allocators.end())
		{
			allocator = allocators[memoryType] = new AllocatorBuddyBlock(sizeof(T) * 100);
		}
		auto target = allocator->second->AddHeapBlock(sizeof(T));
		new(target) T(std::forward<N>(args)...);
		//return new T(std::forward<N>(args)...);
	}

	template<class T>
	void Delete(T* target)
	{
		delete target;
	}

private:
	std::map<uint32_t, AllocatorBuddyBlock*> allocators;
};