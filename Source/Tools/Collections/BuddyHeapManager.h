#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <Tools\Collections\LinkedList.h>
#include <mutex>

struct HeapBlock
{
	HeapBlock()
	{
		address = INT_MAX;
		size = INT_MAX;
	}

	HeapBlock(uint64_t address, size_t size)
		: address(address)
		, size(size)
	{
	}
	uint64_t address;
	size_t size;
};

class BuddyHeapManager
{
public:
	BuddyHeapManager(size_t capacity);

	HeapBlock Allocate(size_t size);
	void Deallocate(HeapBlock& block);

private:
	LinkedList<HeapBlock>::Iterator TryFindFreeBlock(size_t size);
	LinkedList<HeapBlock>::Iterator TryFindClosestBlock(HeapBlock& block);

private:
	LinkedList<HeapBlock> freeBlocks;
	size_t capacity;
};