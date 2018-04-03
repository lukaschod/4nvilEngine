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

#pragma once

#include <mutex>
#include <Core/Tools/Common.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Tools/Collections/LinkedList.hpp>

namespace Core
{
    class BuddyHeapManager;

    struct HeapMemory
    {
        HeapMemory()
        {
            address = INT_MAX;
            size = INT_MAX;
        }

        HeapMemory(uint64 address, size_t size)
            : address(address)
            , size(size)
        {
        }

        uint64 address;
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
}