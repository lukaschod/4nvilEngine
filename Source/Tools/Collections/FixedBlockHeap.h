#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <Tools\Collections\IHeap.h>
#include <mutex>

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