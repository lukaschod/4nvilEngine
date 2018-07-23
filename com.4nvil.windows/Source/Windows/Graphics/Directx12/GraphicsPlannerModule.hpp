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

#include <Core/Tools/Enum.hpp>
#include <Core/Foundation/ComputeModule.hpp>
#include <Core/Graphics/IGraphicsModule.hpp>
#include <Windows/Graphics/Directx12/Common.hpp>
#include <Windows/Graphics/Directx12/DescriptorHeap.hpp>
#include <Windows/Graphics/Directx12/CmdAllocatorPool.hpp>
#include <Windows/Graphics/Directx12/CmdQueue.hpp>

using namespace Core::Math;
using namespace Core::Graphics;

namespace Windows::Directx12
{
    struct Texture;
    struct Buffer;
    struct SwapChain;
    struct RenderPass;
    struct ShaderArguments;
    class GraphicsModule;
    class GraphicsExecutorModule;
}

namespace Windows::Directx12
{
    class GraphicsPlannerModule : public ComputeModule
    {
    public:
        BASE_IS(ComputeModule);

        GraphicsPlannerModule(ID3D12Device* device);
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        virtual Void Execute(const ExecutionContext& context) override;
        virtual Bool IsSplittable() const override { return true; }
        virtual UInt GetExecutionSize() override;
        virtual UInt GetSplitExecutionSize() override;
        virtual const Char* GetName() { return "GraphicsPlannerModule"; }

        Void RecRequestSplit();
        Void RecPushDebug(const Char* name);
        Void RecPopDebug();
        Void RecSetTextureState(const Texture* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState);
        Void RecSetBufferState(const Buffer* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState);
        Void RecSetRenderPass(const RenderPass* target, Bool ignoreLoadAction = false);
        Void RecUpdateBuffer(const Buffer* target, UInt32 targetOffset, Range<UInt8> data);
        Void RecCopyBuffer(const Buffer* src, const Buffer* dst, UInt size);
        Void RecPresent(const SwapChain* swapchain);
        Void RecDraw(const DrawDesc& target);
        Void RecSetHeap(const DescriptorHeap** heap);

        Void Reset();
        ID3D12CommandQueue* GetDirectQueue();
        UInt64 GetRecordingBufferIndex();
        UInt64 GetCompletedBufferIndex();

    private:
        inline Directx12::CmdBuffer* ContinueRecording();
        inline Void SplitRecording();
        inline Bool ExecuteCommand(const ExecutionContext& context, Directx12::CmdBuffer* buffer, CommandCode commandCode);

    private:
        CmdQueue* directQueue;
        CmdAllocatorPool* directAllocatorPool;
        List<Directx12::CmdBuffer*> recordedCmdBuffers;

        struct RecingOptimizer
        {
            inline Void MarSetRenderPass(RenderPass* renderPass) { lastRenderPass = renderPass; drawCount = 0; }
            inline Void MarDraw() { drawCount++; }
            inline Void MarSetHeap(DescriptorHeap** heaps) { memcpy((Void*) lastHeaps, (Void*) heaps, sizeof(DescriptorHeap*) * Enum::ToUnderlying(HeapType::Count)); }
            inline Bool ShouldSplitRecording() { return drawCount == 500; }

            RenderPass* lastRenderPass;
            DescriptorHeap* lastHeaps[Enum::ToUnderlying(HeapType::Count)];
            UInt drawCount;
        };
        RecingOptimizer recordingOptimizer;

        struct DrawOptimizer
        {
            DrawOptimizer() { Clear(); }
            Void Clear()
            {
                lastPipeline = nullptr;
                lastVertexBuffer = nullptr; memset(rootArguments, 0, sizeof(UINT64) * 30);
            }
            const IShaderPipeline* lastPipeline;
            const IBuffer* lastVertexBuffer;
            UINT64 rootArguments[30];

        };
        List<DrawOptimizer> drawOptimizers;

        GraphicsExecutorModule* executor;
        ID3D12CommandAllocator* commandAllocator;
        ID3D12Device* device;
    };
}