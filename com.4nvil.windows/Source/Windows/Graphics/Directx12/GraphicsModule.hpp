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

#include <Core/Graphics/IGraphicsModule.hpp>
#include <Core/Foundation/MemoryModule.hpp>
#include <Windows/Views/ViewModule.hpp>
#include <Windows/Graphics/Directx12/Common.hpp>
#include <Windows/Graphics/Directx12/DescriptorHeap.hpp>
#include <Windows/Graphics/Directx12/BufferHeap.hpp>

using namespace Core::Math;
using namespace Core::Graphics;

namespace Windows::Directx12
{
    struct RenderPass : public IRenderPass
    {
        RenderPass() : IRenderPass()
        {
            memset(colorDescriptors, 0, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE));
            colorDescriptorsCount = 0;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE colorDescriptors[COLOR_ATTACHMENT_MAX_COUNT];
        D3D12_CPU_DESCRIPTOR_HANDLE depthDescriptor;
        UInt colorDescriptorsCount;
        D3D12_VIEWPORT d12Viewport;
        D3D12_RECT d12ScissorRect;
    };

    struct Filter : public IFilter
    {
        Filter()
        {
        }
        HeapMemory srvMemory;
    };

    struct Texture : public ITexture
    {
        Texture(UInt32 width, UInt32 height)
            : ITexture(width, height)
            , resource(nullptr)
            , currentState(D3D12_RESOURCE_STATE_COPY_DEST)
        {
        }

        ID3D12Resource* resource;
        HeapMemory srvMemory;
        HeapMemory rtvMemory;
        D3D12_RESOURCE_STATES currentState;
    };

    struct ResourceMemory
    {
        ResourceMemory()
            : memory(0, 0)
            , resource(nullptr)
            , state(nullptr)
            , cachedMappedPointer(nullptr)
            , cachedGpuVirtualAddress(0)
        { }

        Void SetState(D3D12_RESOURCE_STATES& newState)
        {
            state = (memory.address == 0) ? (D3D12_RESOURCE_STATES*) newState : &newState;
        }

        D3D12_RESOURCE_STATES GetState()
        {
            return (memory.address == 0) ? static_cast<D3D12_RESOURCE_STATES>((UInt64)state) : *state;
        }

        HeapMemory memory;
        ID3D12Resource* resource;
        D3D12_RESOURCE_STATES* state;
        UInt8* cachedMappedPointer;
        D3D12_GPU_VIRTUAL_ADDRESS cachedGpuVirtualAddress;
    };

    struct SrvMemory
    {
        HeapMemory memory;
    };

    struct RtvMemory
    {
        HeapMemory memory;
    };

    struct Buffer : public IBuffer
    {
        Buffer(UInt size)
            : IBuffer(size)
            , resource(nullptr)
            //, currentState(D3D12_RESOURCE_STATE_COPY_DEST)
            , resourceMappedPointer(nullptr)
            , resourceOffset(0)
        {
        }

        Void SetState(D3D12_RESOURCE_STATES newState)
        {
            if (memory.address == 0)
                state = (Void*) newState;
            else
                *(D3D12_RESOURCE_STATES*) state = newState;
        }

        D3D12_RESOURCE_STATES GetState() const
        {
            return (memory.address == 0) ? (D3D12_RESOURCE_STATES)(UInt64)state : *(D3D12_RESOURCE_STATES*)state;
        }

        HeapMemory memory;
        ID3D12Resource* resource;
        Void* state;
        //D3D12_RESOURCE_STATES currentState;
        D3D12_GPU_VIRTUAL_ADDRESS cachedResourceGpuVirtualAddress;
        UInt8* resourceMappedPointer;
        UInt64 resourceOffset;
    };

    struct RootSubParamter
    {
        RootSubParamter(const Char* name, D3D12_DESCRIPTOR_RANGE_TYPE type, Bool isTexture = false)
            : name(name)
            , type(type)
            , isTexture(isTexture)
        {
        }
        const D3D12_DESCRIPTOR_RANGE_TYPE type;
        const Char* const name;
        const Bool isTexture;
    };

    enum class RootParamterType
    {
        TableSRV,
        TableSamplers,
        ConstantBuffer,
    };

    struct RootParamter
    {
        RootParamter() {}
        RootParamter(RootParamterType type) : type(type) {}

        static RootParamter AsConstantBuffer(const Char* name)
        {
            RootParamter parameter(RootParamterType::ConstantBuffer);
            parameter.supParameters.push_back(RootSubParamter(name, (D3D12_DESCRIPTOR_RANGE_TYPE) -1));
            return parameter;
        }

        Void GetCounts(UInt32* srvCount, UInt32* samplersCount) const
        {
            for (auto& supParameter : supParameters)
            {
                switch (supParameter.type)
                {
                case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                    if (srvCount != nullptr)
                        (*srvCount)++;
                    break;
                case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
                    if (samplersCount != nullptr)
                        (*samplersCount)++;
                    break;
                }
            }
        }

        RootParamterType type;
        List<RootSubParamter> supParameters;
    };

    struct ShaderPipeline : public IShaderPipeline
    {
        ShaderPipeline(const ShaderPipelineDesc* desc)
            : IShaderPipeline(desc)
        {
        }

        RootParamter& FindRootParameter(RootParamterType type)
        {
            for (auto& rootParameter : rootParameters)
            {
                if (rootParameter.type == type)
                    return rootParameter;
            }
            rootParameters.push_back(RootParamter(type));
            return rootParameters.back();
        }

        List<RootParamter> rootParameters;
        ID3D12PipelineState* pipelineState;
        ID3D12RootSignature* rootSignature;
        D3D12_VERTEX_BUFFER_VIEW vertexBuffer;
    };

    struct RootArgument
    {
        RootArgument(HeapMemory& memory)
            : memory(memory)
            , IsCurrentlyUsedByDraw(false)
        {
            subData = new UInt64(memory.size);
            memset(subData, 0, sizeof(UInt64) * memory.size);
        }

        RootArgument(UInt64 data) :
            IsCurrentlyUsedByDraw(false)
        {
            subData = (UInt64*) data;
        }

        HeapMemory memory;
        UInt64* subData;
        Bool IsCurrentlyUsedByDraw;
    };

    struct ShaderArguments : public IShaderArguments
    {
        ShaderArguments(const IShaderPipeline* pipeline) :
            IShaderArguments(pipeline)
        {
        }

        List<RootArgument> rootArguments;
    };

    struct SwapChain : public ISwapChain
    {
        SwapChain(const IView* view) 
            : ISwapChain(view)
            , bacBufferIndex(0)
            , bacBuffers(nullptr)
            , IDXGISwapChain3(nullptr)
        {
        }

        inline IDXGISwapChain3* GetIDXGISwapChain3() const { return IDXGISwapChain3; }
        inline Texture* GetBacBuffer() const { return bacBuffers[bacBufferIndex]; }

        Texture** bacBuffers;
        UInt32 bacBufferIndex;
        UInt32 width;
        UInt32 height;

        IDXGISwapChain3* IDXGISwapChain3;
    };

    struct BlitCopyDesc : public DrawDesc
    {
        RenderPass* renderPass;
        Filter* filter;
    };

    class GraphicsPlannerModule;

    class GraphicsModule : public IGraphicsModule
    {
    public:
        BASE_IS(IGraphicsModule);

        GraphicsModule();
        virtual Void Execute(const ExecutionContext& context) override;
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        virtual const Char* GetName() { return "Directx12::GraphicsModule"; }
        virtual const IBuffer* AllocateBuffer(UInt size) override;
        virtual const ITexture* AllocateTexture(UInt32 width, UInt32 height) override;
        virtual const IFilter* AllocateFilter() override;
        virtual const ISwapChain* AllocateSwapChain(const IView* view) override;
        virtual const IRenderPass* AllocateRenderPass() override;

    public:
        virtual Void RecCreateITexture(const ExecutionContext& context, const ITexture* target) override;

        virtual Void RecCreateIFilter(const ExecutionContext& context, const IFilter* target) override;

        virtual Void RecCreateIRenderPass(const ExecutionContext& context, const IRenderPass* target) override;
        virtual Void RecSetColorAttachment(const ExecutionContext& context, const IRenderPass* target, UInt32 index, const ColorAttachment& attachment) override;
        virtual Void RecSetDepthAttachment(const ExecutionContext& context, const IRenderPass* target, const DepthAttachment& attachment) override;
        virtual Void RecSetViewport(const ExecutionContext& context, const IRenderPass* target, const Viewport& viewport) override;
        virtual Void RecSetRenderPass(const ExecutionContext& context, const IRenderPass* target) override;

        virtual const IShaderPipeline* RecCreateIShaderPipeline(const ExecutionContext& context, const ShaderPipelineDesc* desc) override;
        virtual const IShaderArguments* RecCreateIShaderArguments(const ExecutionContext& context, const IShaderPipeline* pipeline) override;
        virtual Void RecSetTexture(const ExecutionContext& context, const IShaderArguments* properties, const Char* name, const ITexture* texture) override;
        virtual Void RecSetFilter(const ExecutionContext& context, const IShaderArguments* properties, const Char* name, const IFilter* filter) override;
        virtual Void RecSetBuffer(const ExecutionContext& context, const IShaderArguments* properties, const Char* name, const IBuffer* buffer) override;

        virtual Void RecCreateIBuffer(const ExecutionContext& context, const IBuffer* target) override;
        virtual Void RecSetBufferUsage(const ExecutionContext& context, const IBuffer* target, BufferUsageFlags usage) override;
        virtual Void RecUpdateBuffer(const ExecutionContext& context, const IBuffer* target, Void* data, UInt size) override;
        virtual Void RecCopyBuffer(const ExecutionContext& context, const IBuffer* src, const IBuffer* dst, UInt size) override;

        virtual Void RecCreateISwapChain(const ExecutionContext& context, const ISwapChain* target) override;
        virtual Void RecPresent(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) override;
        virtual Void RecFinalBlit(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) override;

        virtual Void RecPushDebug(const ExecutionContext& context, const Char* name) override;
        virtual Void RecPopDebug(const ExecutionContext& context) override;

        virtual Void RecDraw(const ExecutionContext& context, const DrawDesc& target) override;

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        inline Bool Initialize();
        inline Void InitializeSwapCain(SwapChain* swapChain);
        inline Void InitializeTexture(Texture* texure);
        inline Void InitializeFilter(Filter* filter);
        inline Void InitializeRenderPass(RenderPass* renderPass);
        inline Void InitializeBuffer(Buffer* buffer);
        inline Void InitializePipeline(ShaderPipeline* pipeleine);
        inline Void CompilePipeline(ShaderPipeline* pipeleine);
        inline Void InitializeProperties(ShaderArguments* target);
        inline Void InitializeBlitCopy(BlitCopyDesc* target);

        DXGI_FORMAT Convert(ColorFormat format);
        const Char* Convert(VertexAttributeType type);
        UInt32 GetSize(ColorFormat format);

        inline Void Present(const ExecutionContext& context, SwapChain* swapChain, Texture* offscreen);
        inline Void BlitCopy(const ExecutionContext& context, Texture* src, Texture* dest);
        inline Void SetTextureState(const ExecutionContext& context, Texture* target, D3D12_RESOURCE_STATES state);
        inline Void SetBufferState(const ExecutionContext& context, Buffer* target, D3D12_RESOURCE_STATES state);
        inline Void SetBuffer(ShaderArguments* target, const Char* name, const Buffer* buffer);
        inline Void SetTexture(ShaderArguments* target, const Char* name, const Texture* texture);
        inline Void SetFilter(ShaderArguments* target, const Char* name, const Filter* filter);
        inline Void SetColorAttachment(const ExecutionContext& context, RenderPass* target, UInt32 index, const ColorAttachment& attachment);
        inline Void SetDepthAttachment(const ExecutionContext& context, RenderPass* target, const DepthAttachment& attachment);
        inline Void SetViewport(const ExecutionContext& context, RenderPass* target, const Viewport& viewport);
        inline Void SetRenderPass(const ExecutionContext& context, const RenderPass* target);
        inline Void UpdateBuffer(Buffer* target, UInt32 targetOffset, Range<UInt8> data);
        inline Void Draw(const ExecutionContext& context, const DrawDesc& target);
        inline Void SetName(ID3D12Object* object, const wchar_t* format, ...);

    private:
        GraphicsPlannerModule* planner;
        DescriptorHeap* srvHeap;
        DescriptorHeap* rtvHeap;
        DescriptorHeap* samplersHeap;
        BufferHeap* bufferUploadHeap;
        BufferHeap* bufferDefaultHeap;
        List<std::pair<UInt64, HeapMemory>> srvHeapMemoryToFree;
        List<std::pair<UInt64, HeapMemory>> samplersHeapMemoryToFree;
        UInt64 resourceCounter;
        DescriptorHeap* srvCpuHeap;
        DescriptorHeap* samplersCpuHeap;

        ID3D12Device* device;
        IDXGIFactory4* factory;
        D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureFeatures;

        MemoryModule* memoryModule;
    };
}