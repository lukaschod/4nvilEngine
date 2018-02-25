#pragma once

#include <mutex>
#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <Tools\Collections\IHeap.h>

namespace Core
{
	class FixedBlockHeap : public IHeap
	{
	public:
		FixedBlockHeap(size_t elementSize);

		virtual void* Allocate() override;
		virtual void Deallocate(void* pointer) override;

	private:
		struct BlockHeader
		{
			BlockHeader() {}
			BlockHeader(uint8* pointer) : pointer(pointer) {}
			uint8* pointer;
		};

		void AddHeapBlock(size_t size);

	private:
		List<BlockHeader> freeBlockHeaders;
		List<void*> totalBlocks;
		size_t capacity;
		size_t elementSize;

		std::mutex allocationMutex;
	};
}