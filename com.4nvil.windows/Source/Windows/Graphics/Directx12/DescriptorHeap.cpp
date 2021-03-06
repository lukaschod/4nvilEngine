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

#include <Windows/Graphics/Directx12/DescriptorHeap.hpp>

using namespace Windows::Directx12;

DescriptorHeap::DescriptorHeap(ID3D12Device* device, HeapType type, UInt capacity) 
    : device(device)
    , type(type)
{
    switch (type)
    {
    case HeapType::SRVs:
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        alignment = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
        break;
    case HeapType::Samplers:
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        alignment = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
        break;
    case HeapType::RTVs:
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        alignment = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
        break;
    case HeapType::SRVsCPU:
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        alignment = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
        break;
    case HeapType::SamplersCPU:
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        alignment = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
        break;
    }

    Grow(capacity);
    Allocate(1); // Reserve for null pointer
}

DescriptorHeap::~DescriptorHeap()
{
    for (int i = 0; i < heapManagers.size(); i++)
    {
        delete heapManagers[i];
        descriptorHeaps[i]->Release();
    }
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGpuHandle(const HeapMemory& memory) const
{
    auto index = FindIndex(memory);
    auto desciptorHeap = descriptorHeaps[index];
    auto handle = desciptorHeap->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += memory.address * alignment;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCpuHandle(const HeapMemory& memory) const
{
    auto index = FindIndex(memory);
    auto desciptorHeap = descriptorHeaps[index];
    auto handle = desciptorHeap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += memory.address * alignment;
    return handle;
}

HeapMemory DescriptorHeap::Allocate(UInt size)
{
    for (auto heapManager : heapManagers)
    {
        auto address = heapManager->Allocate(size);
        if (address.size != 0)
            return address;
    }

    // Grow and repeat
    while (true)
    {
        Grow(capacity);
        auto heapManager = heapManagers.back();
        auto address = heapManager->Allocate(size);
        if (address.size != 0)
            return address;
    }
}

Void DescriptorHeap::Deallocate(const HeapMemory& memory)
{
    for (auto heapManager : heapManagers)
        if (heapManager->Deallocate(memory))
            return;
    ERROR("Invalid HeapMemory");
}

Void DescriptorHeap::Grow(UInt capacity)
{
    ID3D12DescriptorHeap* heap;
    srvHeapDesc.NumDescriptors = (UINT) capacity;
    ASSERT_SUCCEEDED((device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&heap))));
    descriptorHeaps.push_back(heap);

    heapManagers.push_back(new BuddyHeapManager(HeapMemory(this->capacity, capacity)));
    this->capacity += capacity;
}

int DescriptorHeap::FindIndex(const HeapMemory& memory) const
{
    for (int i = 0; i < heapManagers.size(); i++)
    {
        if (heapManagers[i]->Contains(memory))
            return i;
    }

    ERROR("Invalid HeapMemory");
    return -1;
}

