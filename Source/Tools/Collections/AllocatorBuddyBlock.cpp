#include <Tools\Collections\AllocatorBuddyBlock.h>
#include <Tools\Math\Math.h>

AllocatorBuddyBlock::AllocatorBuddyBlock(size_t capacity)
{
	begin.size = 0;
	begin.prev = nullptr;
	begin.next = &end;

	end.size = 0;
	end.prev = &begin;
	end.next = nullptr;

	AddHeapBlock(capacity);
}

void* AllocatorBuddyBlock::Allocate(size_t size)
{
	auto block = TryFindBlock(size);

	// Grow heap if no free block is found
	if (block == &end)
	{
		auto powerBy = Math::Log2(size) + 1;
		auto newBlockSize = Math::Pow2(powerBy);
		AddHeapBlock(newBlockSize);
		block = TryFindBlock(size);
	}

	// Shrink the block
	if (block->size > size)
	{
		auto shrinkedBlock = (BlockHeader*)(block + sizeof(BlockHeader) + size);
		shrinkedBlock->size = block->size - size;
		shrinkedBlock->prev = block->prev;
		shrinkedBlock->next = block->next;

		block->size = size;

		return shrinkedBlock;
	}

	// Remove block
	Connect(block->prev, block->next);
	block->size = size;
	return block;
}

void AllocatorBuddyBlock::Free(void* pointer)
{
}

AllocatorBuddyBlock::BlockHeader* AllocatorBuddyBlock::AddHeapBlock(size_t size)
{
	auto block = malloc(size);
	totalBlocks.push_back(block);

	auto header = (BlockHeader*) block;
	header->size = size;
	Connect(end.prev, header);
	Connect(header, &end);

	return header;
}

AllocatorBuddyBlock::BlockHeader* AllocatorBuddyBlock::TryFindBlock(size_t size)
{
	for (auto iterator = begin.next; iterator != &end; iterator = iterator->next)
	{
		if (iterator->size >= size)
			return iterator;
	}
	return &end;
}

void AllocatorBuddyBlock::Connect(BlockHeader* first, BlockHeader* second)
{
	first->next = second;
	second->prev = first;
}
