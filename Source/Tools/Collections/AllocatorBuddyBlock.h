/*#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <mutex>

class IAllocatorMemory
{
public:
	virtual void Allocate(size_t size) = 0;
	virtual void Deallocate(size_t size) = 0;

};

class AllocatorBuddyBlock
{
public:
	AllocatorBuddyBlock(size_t capacity);

	void* Allocate(size_t size);
	void Deallocate(void* pointer);

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