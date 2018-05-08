/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <Core/Tools/Collections/BuddyHeapManager.hpp>

using namespace Core;

BuddyHeapManager::BuddyHeapManager(const HeapMemory& bounds)
    : bounds(bounds)
{
    freeBlocks.begin().link->value = HeapMemory(0, 0); // Reserve null for begin
    freeBlocks.end().link->value = HeapMemory(INT64_MAX, 0); // Reserver max for end

    ASSERT(bounds.size != 0);
    freeBlocks.Add(bounds);
}

HeapMemory BuddyHeapManager::Allocate(UInt size)
{
    ASSERT(size != 0);

    auto itr = TryFindFreeBlock(size);
    if (itr == freeBlocks.end())
        return HeapMemory(0, 0);

    auto& block = *itr;
    auto blockSize = block.size;

    if (blockSize < size)
        return HeapMemory(0, 0);

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

Bool BuddyHeapManager::Deallocate(const HeapMemory& block)
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
            secondBlock.address += firstBlock.size + block.size;
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
            secondBlock.address -= block.size;
            secondBlock.size += block.size;
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

Bool BuddyHeapManager::Contains(const HeapMemory& memory)
{
    return bounds.address <= memory.address && memory.address + memory.size <= bounds.address + bounds.size;
}

LinkedList<HeapMemory>::Iterator BuddyHeapManager::TryFindFreeBlock(UInt size)
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
        if (firstBlock.address + firstBlock.size <= block.address && block.address + block.size <= secondBlock.address)
            return itr;
    }
    return itr;
}
