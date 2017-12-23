#include <Tools\Collections\AllocatorFixedBlock.h>
#include <Tools\Math\Math.h>

AllocatorFixedBlock::AllocatorFixedBlock(size_t elementSize)
	: elementSize(elementSize)
	, capacity(0)
{
	AddHeapBlock(128);
}

void* AllocatorFixedBlock::Allocate()
{
	std::lock_guard<std::mutex> lock(allocationMutex);

	// Grow heap if no free block is found
	if (freeBlockHeaders.empty())
		AddHeapBlock(capacity);

	auto pointer = freeBlockHeaders.back().pointer;
	freeBlockHeaders.pop_back();
	return pointer;
}

void AllocatorFixedBlock::Free(void* pointer)
{
	std::lock_guard<std::mutex> lock(allocationMutex);
	freeBlockHeaders.push_back(BlockHeader((uint8_t*)pointer));
}

void AllocatorFixedBlock::AddHeapBlock(size_t count)
{
	if (freeBlockHeaders.empty())
	{
		auto size = count * elementSize;
		auto block = (uint8_t*) malloc(size);
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

	ERROR("TODO");
}