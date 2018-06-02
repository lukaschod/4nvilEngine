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
#include <Core/Tools/Enum.hpp>
#include <Core/Tools/IO/MemoryStream.hpp>
#include <Windows/Graphics/Directx12/Common.hpp>
#include <Windows/Graphics/Directx12/DescriptorHeap.hpp>
#include <Windows/Graphics/Directx12/CmdAllocatorPool.hpp>

using namespace Core;

namespace Windows::Directx12
{
    class CmdQueue;
}

namespace Windows::Directx12
{
    struct CmdBuffer
    {
        CmdBuffer(CmdQueue* queue, D3D12_COMMAND_LIST_TYPE type)
            : type(type)
            , queue(queue)
            , commandList(nullptr)
        {
        }

        Void Reset(Int64 index)
        {
            commandCount = 0;
            this->index = index;
            swapChain = nullptr;
            stream.Reset();
            memset(heaps, 0, Enum::ToUnderlying(HeapType::Count) * sizeof(DescriptorHeap*));
        }

        const D3D12_COMMAND_LIST_TYPE type;
        CmdQueue* const queue;
        IO::MemoryStream stream;
        UInt commandCount;
        ID3D12GraphicsCommandList* commandList;
        IDXGISwapChain* swapChain;
        UInt64 index;
        DescriptorHeap* heaps[Enum::ToUnderlying(HeapType::Count)];
    };

    class CmdQueue
    {
    public:
        CmdQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
        ~CmdQueue();

        CmdBuffer* Pull();
        Void Push(CmdBuffer* buffer);
        Void Reset(CmdBuffer* buffer, ID3D12CommandAllocator* allocator);
        Void Close(CmdBuffer* buffer);
        Void Execute(CmdBuffer* buffer, Bool isLast);

        UInt64 GetCompletedBufferIndex();
        Void WaitForBufferIndexToComplete(UInt64 index);
        inline ID3D12CommandQueue* GetQueue() { return queue; }

    private:
        const D3D12_COMMAND_LIST_TYPE type;
        ID3D12Device* const device;
        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE waitBufferCompletionEvent;
        std::mutex waitBufferCompletionMutex;

        List<CmdBuffer*> buffers;
        std::queue<CmdBuffer*> readyBuffers;
        UInt64 pulledBufferCount;

        List<ID3D12CommandList*> cmdsToExecute;
    };
}