#pragma once

#include <map>
#include <Tools\Common.h>
#include <Tools\Collections\FixedBlockHeap.h>
#include <Modules\Module.h>

namespace Core
{
	class MemoryModule : public Module
	{
	public:
		virtual void Execute(const ExecutionContext& context) override {}

		template<class T, class ... N>
		T* New(const char* memoryType, N ... args)
		{
			auto allocator = allocators.find(memoryType);
			ASSERT_MSG(allocator != allocators.end(), "Allocator with memory type %d is not set", memoryType);
			auto target = (T*) allocator->second->Allocate();
			new(target) T(std::forward<N>(args)...);
			return target;
		}

		void SetAllocator(const char* memoryType, IHeap* allocator)
		{
			auto allocatorDup = allocators.find(memoryType);
			ASSERT_MSG(!(allocatorDup != allocators.end() && typeid(allocatorDup->second) != typeid(allocator)),
				"Allocator was already set for memory type %d", memoryType);
			allocators[memoryType] = allocator;
		}

		template<class T>
		void Delete(const char* memoryType, T* target)
		{
			auto allocator = allocators.find(memoryType);
			ASSERT_MSG(allocator != allocators.end(), "Allocator with memory type %d is not set", memoryType);
			allocator->second->Deallocate(target);
			target->~T();
		}

	private:
		std::map<const char*, IHeap*> allocators;
	};
}