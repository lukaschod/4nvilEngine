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

#include <queue>
#include <mutex>
#include <Windows\Graphics\Directx12\Common.hpp>

namespace Windows::Directx12
{
    // Taken from sample and refactored to project syntax
    class CmdAllocatorPool
    {
    public:
        CmdAllocatorPool(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
        ~CmdAllocatorPool();

        ID3D12CommandAllocator* TryPull(uint64 completedFenceValue);
        void Push(uint64 fenceValue, ID3D12CommandAllocator* allocator);

    private:
        const D3D12_COMMAND_LIST_TYPE type;
        ID3D12Device* const device;
        std::vector<ID3D12CommandAllocator*> allocatorPool;
        std::queue<std::pair<uint64, ID3D12CommandAllocator*>> readyAllocators;
        std::mutex allocatorMutex;
    };
}