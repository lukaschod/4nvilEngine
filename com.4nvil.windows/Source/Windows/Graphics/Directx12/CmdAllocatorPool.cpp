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

#include <Windows\Graphics\Directx12\CmdAllocatorPool.hpp>

using namespace Windows::Directx12;

CmdAllocatorPool::CmdAllocatorPool(ID3D12Device * device, D3D12_COMMAND_LIST_TYPE type)
    : device(device)
    , type(type)
{
}

CmdAllocatorPool::~CmdAllocatorPool()
{
    for (auto allocator : allocatorPool)
        allocator->Release();
    allocatorPool.clear();
}

ID3D12CommandAllocator* CmdAllocatorPool::TryPull(uint64 completedFenceValue)
{
    std::lock_guard<std::mutex> lock(allocatorMutex);

    ID3D12CommandAllocator* allocator = nullptr;

    if (!readyAllocators.empty())
    {
        std::pair<uint64, ID3D12CommandAllocator*>& allocatorPair = readyAllocators.front();

        if (allocatorPair.first <= completedFenceValue)
        {
            allocator = allocatorPair.second;
            ASSERT_SUCCEEDED(allocator->Reset());
            readyAllocators.pop();
        }
    }

    // If no allocator's were ready to be reused, create a new one
    if (allocator == nullptr)
    {
        ASSERT_SUCCEEDED(device->CreateCommandAllocator(type, IID_PPV_ARGS(&allocator)));
        wchar_t allocatorName[32];
        swprintf(allocatorName, 32, L"CommandAllocator %zu", allocatorPool.size());
        allocator->SetName(allocatorName);
        allocatorPool.push_back(allocator);
    }

    return allocator;
}

void CmdAllocatorPool::Push(uint64 fenceValue, ID3D12CommandAllocator* allocator)
{
    std::lock_guard<std::mutex> lock(allocatorMutex);

    // That fence value indicates we are free to reset the allocator
    readyAllocators.push(std::make_pair(fenceValue, allocator));
}
