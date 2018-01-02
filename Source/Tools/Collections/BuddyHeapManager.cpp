#include <Tools\Collections\BuddyHeapManager.h>

BuddyHeapManager::BuddyHeapManager(size_t capacity)
	: capacity(capacity)
{
	ASSERT(capacity != 0);
	freeBlocks.Add(HeapBlock(0, capacity));
}

HeapBlock BuddyHeapManager::Allocate(size_t size)
{
	ASSERT(size != 0);

	auto itr = TryFindFreeBlock(size);
	if (itr == freeBlocks.end())
		return HeapBlock(0, 0);

	auto& block = *itr;
	auto blockSize = block.size;

	if (blockSize == size)
	{
		freeBlocks.Remove(itr);
		return block;
	}

	freeBlocks.Insert(itr, HeapBlock(block.address + size, blockSize - size));
	return HeapBlock(block.address, size);
}

void BuddyHeapManager::Deallocate(HeapBlock& block)
{
	ASSERT(0 <= block.address && block.address + block.size <= capacity);
	ASSERT(block.size != 0);

	auto itr = TryFindClosestBlock(block);
	if (itr == freeBlocks.end())
		return;

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
			secondBlock.address -= firstBlock.size;
			secondBlock.size += firstBlock.size + block.size;
			freeBlocks.Remove(itr);
			return;
		}
		else
		{
			firstBlock.size += block.size;
			return;
		}
	}
	else
	{
		if (isBottomOverlaps)
		{
			secondBlock.address -= block.size;
			return;
		}
		else
		{
			freeBlocks.Insert(itr, block);
			return;
		}
	}
}

LinkedList<HeapBlock>::Iterator BuddyHeapManager::TryFindFreeBlock(size_t size)
{
	auto& itr = freeBlocks.begin();
	for (; itr != freeBlocks.end(); ++itr)
	{
		if ((*itr).size >= size)
			return itr;
	}
	return itr;
}

LinkedList<HeapBlock>::Iterator BuddyHeapManager::TryFindClosestBlock(HeapBlock& block)
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
