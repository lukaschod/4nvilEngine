#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <mutex>

class IAllocator
{
public:
	virtual void* Allocate() = 0;
	virtual void Free(void* pointer) = 0;
};

class AllocatorFixedBlock : public IAllocator
{
public:
	AllocatorFixedBlock(size_t elementSize);

	virtual void* Allocate() override;
	virtual void Free(void* pointer) override;

private:
	struct BlockHeader
	{
		BlockHeader() {}
		BlockHeader(uint8_t* pointer) : pointer(pointer) {}
		uint8_t* pointer;
	};

	void AddHeapBlock(size_t size);

private:
	List<BlockHeader> freeBlockHeaders;
	List<void*> totalBlocks;
	size_t capacity;
	size_t elementSize;

	std::mutex allocationMutex;
};