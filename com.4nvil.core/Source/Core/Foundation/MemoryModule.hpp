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

#include <Core/Tools/Common.hpp>
#include <Core/Tools/Collections/FixedBlockHeap.hpp>
#include <Core/Modules/Module.hpp>
#include <map>

namespace Core
{
	// TODO: memorytype needs actual type
    class MemoryModule : public Module
    {
    public:
        virtual Void Execute(const ExecutionContext& context) override {}

        template<class T, class ... N>
        T* New(const Char* memoryType, N ... args)
        {
            auto allocator = allocators.find(memoryType);
            ASSERT_MSG(allocator != allocators.end(), "Allocator with memory type %d is not set", memoryType);
            auto target = (T*) allocator->second->Allocate();
            new(target) T(std::forward<N>(args)...);
            return target;
        }

		// TODO: Probably we should change this to automatic allocator creator, to avoid pointer passing that could be destroyed externaly
        Void SetAllocator(const Char* memoryType, IHeap* allocator)
        {
            auto allocatorDup = allocators.find(memoryType);
            ASSERT_MSG(!(allocatorDup != allocators.end() && typeid(allocatorDup->second) != typeid(allocator)),
                "Allocator was already set for memory type %d", memoryType);
            allocators[memoryType] = allocator;
        }

        template<class T>
        Void Delete(const Char* memoryType, T* target)
        {
            auto allocator = allocators.find(memoryType);
            ASSERT_MSG(allocator != allocators.end(), "Allocator with memory type %d is not set", memoryType);
            allocator->second->Deallocate(target);
            target->~T();
        }

    private:
        std::map<const Char*, IHeap*> allocators;
    };
}