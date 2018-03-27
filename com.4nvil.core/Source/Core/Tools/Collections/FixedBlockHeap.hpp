#pragma once

#include <mutex>
#include <Core\Tools\Common.hpp>
#include <Core\Tools\Collections\List.hpp>
#include <Core\Tools\Collections\IHeap.hpp>

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