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

#include <Core/Tools/Math/Math.hpp>
#include <Windows/Graphics/Directx12/BufferHeap.hpp>

using namespace Windows::Directx12;

BufferHeap::BufferHeap(ID3D12Device* device, UInt capacity, UInt alignment, D3D12_HEAP_TYPE type)
    : device(device)
    , alignment(alignment)
    , type(type)
{
    Grow(capacity);
    Allocate(alignment); // Reserve for null pointer
}

BufferHeap::~BufferHeap()
{
    for (int i = 0; i < heapManagers.size(); i++)
    {
        delete heapManagers[i];
        resources[i]->Release();
    }
}

D3D12_GPU_VIRTUAL_ADDRESS BufferHeap::GetVirtualAddress(const HeapMemory& memory) const
{
    auto index = FindIndex(memory);
    return resources[index]->GetGPUVirtualAddress() + memory.address - heapManagers[index]->GetBounds().address;
}

ID3D12Resource* BufferHeap::GetResource(const HeapMemory& memory) const
{
    auto index = FindIndex(memory);
    return resources[index];
}

UInt8* BufferHeap::GetResourceMappedPointer(const HeapMemory& memory) const
{
    auto index = FindIndex(memory);
    return resourceMappedPointers[index] + memory.address - heapManagers[index]->GetBounds().address;
}

D3D12_RESOURCE_STATES* BufferHeap::GetState(const HeapMemory& memory) const
{
    auto index = FindIndex(memory);
    return (D3D12_RESOURCE_STATES*)states.data() + index;
}

UInt64 BufferHeap::GetResourceOffset(const HeapMemory & memory) const
{
    auto index = FindIndex(memory);
    return memory.address - heapManagers[index]->GetBounds().address;
}

HeapMemory BufferHeap::Allocate(UInt size)
{
    size = Math::GetPadded(size, alignment);
    while (true)
    {
        auto heapManager = heapManagers.back();
        auto address = heapManager->Allocate(size);
        if (address.size == 0)
        {
            Grow(capacity);
            continue;
        }
        return address;
    }
}

Void BufferHeap::Deallocate(const HeapMemory& memory)
{
    for (auto heapManager : heapManagers)
        if (heapManager->Deallocate(memory))
            return;
    ERROR("Invalid HeapMemory");
}

Void BufferHeap::Grow(UInt capacity)
{
    auto state = D3D12_RESOURCE_STATE_GENERIC_READ;

    ID3D12Resource* resource;
    ASSERT_SUCCEEDED(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(type),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(capacity),
        state,
        nullptr,
        IID_PPV_ARGS(&resource)));
    resources.push_back(resource);

    // TODO: Clean it a bit, currently lets keep it that way to check if we win anything by always keeping resource mapped
    if (type == D3D12_HEAP_TYPE_UPLOAD)
    {
        UInt8* resourceMappedPointer;
        CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
        ASSERT_SUCCEEDED(resource->Map(0, &readRange, reinterpret_cast<Void**>(&resourceMappedPointer)));
        resourceMappedPointers.push_back(resourceMappedPointer);
    }

    states.push_back(state);

    heapManagers.push_back(new BuddyHeapManager(HeapMemory(this->capacity, capacity)));
    this->capacity += capacity;
}

int BufferHeap::FindIndex(const HeapMemory& memory) const
{
    for (int i = 0; i < heapManagers.size(); i++)
    {
        if (heapManagers[i]->Contains(memory))
            return i;
    }

    ERROR("Invalid HeapMemory");
    return -1;
}
