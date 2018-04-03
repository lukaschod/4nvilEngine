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
#include <Core/Tools/Collections/IHeap.hpp>

namespace Core
{
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
            BlockHeader(uint8* pointer) : pointer(pointer) {}
            uint8* pointer;
        };

        void AddHeapBlock(size_t size);

    private:
        List<BlockHeader> freeBlockHeaders;
        List<void*> totalBlocks;
        size_t capacity;
        size_t elementSize;

        std::mutex allocationMutex;
    };
}