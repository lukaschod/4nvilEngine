#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <Tools\Collections\LinkedList.h>
#include <mutex>

class BuddyHeapManager;

struct HeapMemory
{
	HeapMemory()
	{
		address = INT_MAX;
		size = INT_MAX;
	}

	HeapMemory(uint64_t address, size_t size)
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
	BuddyHeapManager(const HeapMemory& bounds);

	HeapMemory Allocate(size_t size);
	bool Deallocate(const HeapMemory& memory);
	bool Contains(const HeapMemory& memory);
	const HeapMemory& GetBounds() const { return bounds; }

private:
	LinkedList<HeapMemory>::Iterator TryFindFreeBlock(size_t size);
	LinkedList<HeapMemory>::Iterator TryFindClosestBlock(const HeapMemory& block);

private:
	LinkedList<HeapMemory> freeBlocks;
	HeapMemory bounds;
};