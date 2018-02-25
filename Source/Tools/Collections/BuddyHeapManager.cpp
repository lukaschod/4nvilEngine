#include <Tools\Collections\BuddyHeapManager.h>

using namespace Core;

BuddyHeapManager::BuddyHeapManager(const HeapMemory& bounds)
	: bounds(bounds)
{
	ASSERT(bounds.size != 0);
	freeBlocks.Add(bounds);
}

HeapMemory BuddyHeapManager::Allocate(size_t size)
{
	ASSERT(size != 0);

	auto itr = TryFindFreeBlock(size);
	if (itr == freeBlocks.end())
		return HeapMemory(0, 0);

	auto& block = *itr;
	auto blockSize = block.size;

	if (blockSize == size)
	{
		freeBlocks.Remove(itr);
		return block;
	}

	auto address = block.address;
	block.address += size;
	block.size -= size;
	return HeapMemory(address, size);
}

bool BuddyHeapManager::Deallocate(const HeapMemory& block)
{
	ASSERT(block.size != 0);

	if (!Contains(block))
		return false;

	auto itr = TryFindClosestBlock(block);
	if (itr == freeBlocks.end())
		return false;

	auto& firstBlock = itr.link->value;
	auto& secondBlock = itr.link->next->value;

	auto top = firstBlock.address + firstBlock.size;
	auto bottom = secondBlock.address;

	auto blockTop = block.address;
	auto blockBottom = blockTop + block.size;

	auto isTopOverlaps = (top == blockTop);
	auto isBottomOverlaps = (bottom == blockBottom);

	if (isTopOverlaps)
	{
		if (isBottomOverlaps)
		{
			secondBlock.address += firstBlock.size;
			secondBlock.size += firstBlock.size + block.size;
			freeBlocks.Remove(itr);
			return true;
		}
		else
		{
			firstBlock.size += block.size;
			return true;
		}
	}
	else
	{
		if (isBottomOverlaps)
		{
			secondBlock.address += block.size;
			return true;
		}
		else
		{
			freeBlocks.Insert(itr, block);
			return true;
		}
	}
	return true;
}

bool BuddyHeapManager::Contains(const HeapMemory& memory)
{
	return bounds.address <= memory.address && memory.address + memory.size <= bounds.address + bounds.size;
}

LinkedList<HeapMemory>::Iterator BuddyHeapManager::TryFindFreeBlock(size_t size)
{
	auto& itr = freeBlocks.begin();
	for (; itr != freeBlocks.end(); ++itr)
	{
		if ((*itr).size >= size)
			return itr;
	}
	return itr;
}

LinkedList<HeapMemory>::Iterator BuddyHeapManager::TryFindClosestBlock(const HeapMemory& block)
{
	auto& itr = freeBlocks.begin();
	for (; itr != freeBlocks.end(); ++itr)
	{
		auto& firstBlock = itr.link->value;
		auto& secondBlock = itr.link->next->value;
		if (firstBlock.address + firstBlock.size >= block.address &&
			block.address + block.size <= secondBlock.address)
			return itr;
	}
	return itr;
}
