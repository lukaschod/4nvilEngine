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

#include <Core/Tools/Collections/List.hpp>
#include <Core/Tools/Collections/BuddyHeapManager.hpp>
#include <Windows/Graphics/Directx12/Common.hpp>
#include <Windows/Graphics/Directx12/DescriptorHeap.hpp>

using namespace Core;

namespace Windows::Directx12
{
    class BufferHeap
    {
    public:
        BufferHeap(ID3D12Device* device, UInt capacity, UInt alignment, D3D12_HEAP_TYPE type);
        ~BufferHeap();
        HeapMemory Allocate(UInt size);
        Void Deallocate(const HeapMemory& memory);
        D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress(const HeapMemory& memory) const;
        ID3D12Resource* GetResource(const HeapMemory& memory) const;
        UInt8* GetResourceMappedPointer(const HeapMemory& memory) const;
        D3D12_RESOURCE_STATES* GetState(const HeapMemory& memory) const;
        UInt64 GetResourceOffset(const HeapMemory& memory) const;

    private:
        Void Grow(UInt capacity);
        int FindIndex(const HeapMemory& memory) const;

    private:
        D3D12_HEAP_TYPE type;
        List<BuddyHeapManager*> heapManagers;
        List<ID3D12Resource*> resources;
        List<D3D12_RESOURCE_STATES> states;
        List<UInt8*> resourceMappedPointers;
        ID3D12Device* device;
        UInt capacity;
        UInt alignment;
    };
}