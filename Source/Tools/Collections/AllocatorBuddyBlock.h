#pragma once

/*#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <mutex>

class AllocatorBuddyBlock
{
public:
	AllocatorBuddyBlock(size_t capacity);

	void* Allocate(size_t size);
	void Free(void* pointer);

private:
	struct BlockHeader
	{
		uint32_t size;
		BlockHeader* prev;
		BlockHeader* next;
	};

	BlockHeader* AddHeapBlock(size_t size);
	BlockHeader* TryFindBlock(size_t size);
	void Connect(BlockHeader* first, BlockHeader* second);

private:
	BlockHeader begin;
	BlockHeader end;
	List<void*> totalBlocks;

	std::mutex allocationMutex;
};*/