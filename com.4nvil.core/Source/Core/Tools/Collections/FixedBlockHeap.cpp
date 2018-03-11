#include <Core\Tools\Math\Math.h>
#include <Core\Tools\Collections\FixedBlockHeap.h>

using namespace Core;

FixedBlockHeap::FixedBlockHeap(size_t elementSize)
	: elementSize(elementSize)
	, capacity(0)
{
	elementSize += elementSize % sizeof(void*); // Align according the curent target
	AddHeapBlock(128);
}

void* FixedBlockHeap::Allocate()
{
	std::lock_guard<std::mutex> lock(allocationMutex );

	// Grow heap if no free block is found
	if (freeBlockHeaders.empty())
		AddHeapBlock(capacity);

	auto pointer = freeBlockHeaders.back().pointer;
	freeBlockHeaders.pop_back();
	return pointer;
}

void FixedBlockHeap::Deallocate(void* pointer)
{
	std::lock_guard<std::mutex> lock(allocationMutex);
	freeBlockHeaders.push_back(BlockHeader((uint8*)pointer));
}

void FixedBlockHeap::AddHeapBlock(size_t count)
{
	ASSERT(freeBlockHeaders.empty());

	auto size = count * elementSize;
	auto block = (uint8*) malloc(size);
	totalBlocks.push_back(block);

	auto pointer = block + size - elementSize;
	for (int i = 0; i < count; i++)
	{
		freeBlockHeaders.push_back(BlockHeader(pointer));
		pointer -= elementSize;
	}
	capacity += count;
	return;
}