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

#include <Windows/Graphics/Directx12/GraphicsModule.hpp>
#include <Windows/Graphics/Directx12/GraphicsPlannerModule.hpp>
#include <Windows/Graphics/Directx12/GraphicsExecutorModule.hpp>

using namespace Core;
using namespace Windows::Directx12;

static const Char* memoryLabelShaderArguments = "Core::Graphics::ShaderArguments";
static const Char* memoryLabelBuffer = "Core::Graphics::Buffer";

GraphicsModule::GraphicsModule()
    : device(nullptr)
    , factory(nullptr)
    , resourceCounter(0)
    , blitCopy(nullptr)
{}

Void GraphicsModule::Execute(const ExecutionContext& context)
{
    MARK_FUNCTION;
    // Deallocate all memory that is not used by GPU
    auto completedBufferIndex = planner->GetCompletedBufferIndex();
    for (UInt i = srvHeapMemoryToFree.size(); i-->0 ;)
    {
        auto memory = srvHeapMemoryToFree.back();
        if (memory.first > completedBufferIndex)
            continue;
        srvHeapMemoryToFree.pop_back();
        srvHeap->Deallocate(memory.second);
    }
    for (UInt i = samplersHeapMemoryToFree.size(); i-->0;)
    {
        auto memory = samplersHeapMemoryToFree.back();
        if (memory.first > completedBufferIndex)
            continue;
        samplersHeapMemoryToFree.pop_back();
        samplersHeap->Deallocate(memory.second);
    }

    planner->Reset();
    base::Execute(context);
}

Void GraphicsModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    Initialize();
    moduleManager->AddModule(new GraphicsPlannerModule(device));
    moduleManager->AddModule(new GraphicsExecutorModule());
    planner = ExecuteBefore<GraphicsPlannerModule>(moduleManager);
    memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
    memoryModule->SetAllocator(memoryLabelShaderArguments, new FixedBlockHeap(sizeof(ShaderArguments)));
    memoryModule->SetAllocator(memoryLabelBuffer, new FixedBlockHeap(sizeof(Buffer)));
}

const IBuffer* GraphicsModule::AllocateBuffer(UInt size)
{
    return memoryModule->New<Buffer>(memoryLabelBuffer, size);
}

const ITexture* GraphicsModule::AllocateTexture(UInt32 width, UInt32 height)
{
    return new Texture(width, height);
}

const ISwapChain* GraphicsModule::AllocateSwapChain(const IView * view)
{
    return new SwapChain(view);
}

const IRenderPass* GraphicsModule::AllocateRenderPass()
{
    return new RenderPass();
}

const IFilter* GraphicsModule::AllocateFilter()
{
    return new Filter();
}

SERIALIZE_METHOD_ARG1(GraphicsModule, CreateITexture, const ITexture*);
SERIALIZE_METHOD_ARG1(GraphicsModule, CreateIFilter, const IFilter*);
SERIALIZE_METHOD_ARG1(GraphicsModule, CreateIRenderPass, const IRenderPass*);
SERIALIZE_METHOD_ARG3(GraphicsModule, SetColorAttachment, const IRenderPass*, UInt32, const ColorAttachment&);
SERIALIZE_METHOD_ARG2(GraphicsModule, SetDepthAttachment, const IRenderPass*, const DepthAttachment&);
SERIALIZE_METHOD_ARG2(GraphicsModule, SetViewport, const IRenderPass*, const Viewport&);
SERIALIZE_METHOD_ARG1(GraphicsModule, SetRenderPass, const IRenderPass*);
SERIALIZE_METHOD_CREATEGEN_ARG1(GraphicsModule, IShaderPipeline, ShaderPipeline, const ShaderPipelineDesc*);
SERIALIZE_METHOD_ARG3(GraphicsModule, SetBuffer, const IShaderArguments*, const Char*, const IBuffer*);
SERIALIZE_METHOD_ARG3(GraphicsModule, SetTexture, const IShaderArguments*, const Char*, const ITexture*);
SERIALIZE_METHOD_ARG3(GraphicsModule, SetFilter, const IShaderArguments*, const Char*, const IFilter*);
SERIALIZE_METHOD_ARG2(GraphicsModule, Present, const ISwapChain*, const ITexture*);
SERIALIZE_METHOD_ARG1(GraphicsModule, CreateISwapChain, const ISwapChain*);
SERIALIZE_METHOD_ARG2(GraphicsModule, FinalBlit, const ISwapChain*, const ITexture*);
SERIALIZE_METHOD_ARG1(GraphicsModule, CreateIBuffer, const IBuffer*);
SERIALIZE_METHOD_ARG2(GraphicsModule, SetBufferUsage, const IBuffer*, BufferUsageFlags);
SERIALIZE_METHOD_ARG3(GraphicsModule, UpdateBuffer, const IBuffer*, Void*, UInt);
SERIALIZE_METHOD_ARG3(GraphicsModule, CopyBuffer, const IBuffer*, const IBuffer*, UInt);
SERIALIZE_METHOD_ARG1(GraphicsModule, PushDebug, const Char*);
SERIALIZE_METHOD(GraphicsModule, PopDebug);
SERIALIZE_METHOD_ARG1(GraphicsModule, Draw, const DrawDesc&);

DECLARE_COMMAND_CODE(CreateIShaderArguments);
const IShaderArguments* GraphicsModule::RecCreateIShaderArguments(const ExecutionContext& context, const IShaderPipeline* pipeline)
{
    auto buffer = GetRecordingBuffer(context);
    auto& stream = buffer->stream;
    auto target = memoryModule->New<ShaderArguments>(memoryLabelShaderArguments, pipeline);
    stream.Write(TO_COMMAND_CODE(CreateIShaderArguments));
    stream.Write(target);
    stream.Align();
    buffer->commandCount++;
    return target;
}

Bool GraphicsModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateITexture, Texture*, target);
        InitializeTexture(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(CreateIFilter, Filter*, target);
        InitializeFilter(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(CreateIRenderPass, RenderPass*, target);
        InitializeRenderPass(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(SetColorAttachment, RenderPass*, target, UInt32, index, ColorAttachment, attachment);
        SetColorAttachment(context, target, index, attachment);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetDepthAttachment, RenderPass*, target, DepthAttachment, attachment);
        SetDepthAttachment(context, target, attachment);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetViewport, RenderPass*, target, Viewport, viewport);
        SetViewport(context, target, viewport);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(SetRenderPass, RenderPass*, target);
        SetRenderPass(context, target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(CreateIShaderPipeline, ShaderPipeline*, target);
        InitializePipeline(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(CreateIShaderArguments, ShaderArguments*, target);
        InitializeProperties(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(SetTexture, ShaderArguments*, target, const Char*, name, Texture*, texture);
        SetTexture(target, name, texture);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(SetFilter, ShaderArguments*, target, const Char*, name, Filter*, filter);
        SetFilter(target, name, filter);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(SetBuffer, ShaderArguments*, target, const Char*, name, Buffer*, buffer);
        SetBuffer(target, name, buffer);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(CreateISwapChain, SwapChain*, target);
        InitializeSwapCain(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(Present, SwapChain*, target, Texture*, texture);
        planner->RecPresent(target);
        target->bacBufferIndex = (target->bacBufferIndex + 1) % target->bacBufferCount;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(FinalBlit, SwapChain*, target, Texture*, texture);
        // Blit ofscreen buffer to swapchain backbuffer
        auto bacBuffer = target->GetBacBuffer();
        BlitCopy(context, texture, bacBuffer);

        ASSERT(target->bacBufferIndex == target->IDXGISwapChain3->GetCurrentBackBufferIndex());

        SetTextureState(context, bacBuffer, D3D12_RESOURCE_STATE_PRESENT);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(CreateIBuffer, Buffer*, target);
        InitializeBuffer(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetBufferUsage, Buffer*, target, BufferUsageFlags, usage);
        target->usage = usage;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(UpdateBuffer, Buffer*, target, Void*, data, UInt, size);
        UpdateBuffer(target, 0, Range<UInt8>((const UInt8*)data, size));
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(CopyBuffer, Buffer*, src, Buffer*, dst, UInt, size);
        SetBufferState(context, dst, D3D12_RESOURCE_STATE_COPY_DEST);
        planner->RecCopyBuffer(src, dst, size);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(PushDebug, const Char*, name);
        planner->RecPushDebug(name);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_START(PopDebug);
        planner->RecPopDebug();
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(Draw, DrawDesc, target);
        Draw(context, target);
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Void GraphicsModule::SetTextureState(const ExecutionContext& context, Texture* target, D3D12_RESOURCE_STATES state)
{
    if (target->currentState == state)
        return;

    ASSERT(target->resource != nullptr);
    planner->RecSetTextureState(target, target->currentState, state);
    target->currentState = state;
}

Void GraphicsModule::SetBufferState(const ExecutionContext& context, Buffer* target, D3D12_RESOURCE_STATES state)
{
    /*if (target->currentState == state)
        return;

    ASSERT(target->resource != nullptr);
    planner->RecSetBufferState(target, target->currentState, state);
    target->currentState = state;*/
    if (target->GetState() == state)
        return;

    ASSERT(target->resource != nullptr);
    planner->RecSetBufferState(target, target->GetState(), state);
    target->SetState(state);
}

Void GraphicsModule::SetBuffer(ShaderArguments* target, const Char* name, const Buffer* buffer)
{
    ASSERT(buffer != nullptr);
    auto pipeline = (ShaderPipeline*) target->pipeline;
    auto& rootParameters = pipeline->rootParameters;
    auto& rootArguments = target->rootArguments;
    ASSERT(rootParameters.size() == rootArguments.size());
    for (UInt i = 0; i < rootParameters.size(); i++)
    {
        auto& rootParameter = rootParameters[i];
        switch (rootParameter.type)
        {
        /*case RootParamterType::TableSRV:
        {
            for (UInt j = 0; j < rootParameter.supParameters.size(); j++)
            {
                auto& rootSubParameter = rootParameter.supParameters[j];
                if (strcmp(rootSubParameter.name, name) != 0)
                    continue;

                if (rootArguments[i].subData[j] == (UInt64) buffer)
                    continue;
                rootArguments[i].subData[j] = (UInt64) buffer;

                auto memory = target->rootArguments[i].memory;
                memory.pointer += j;

                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srvDesc.Format = DXGI_FORMAT_UNKNOWN;
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
                srvDesc.Buffer.FirstElement = 0;
                srvDesc.Buffer.NumElements = (UINT) buffer->data.size;
                srvDesc.Buffer.StructureByteStride = 1;
                srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
                device->CreateShaderResourceView(buffer->resource, &srvDesc, srvHeap->GetCpuHandle(memory));
            }
            break;
        }*/

        case RootParamterType::ConstantBuffer:
        {
            auto& rootSubParameter = rootParameter.supParameters[0];
            if (strcmp(rootSubParameter.name, name) != 0)
                continue;

            // TODO: Check if we can really cache the gpu virtual address safely
             rootArguments[i].subData = (UInt64*) buffer->cachedResourceGpuVirtualAddress;
            // rootArguments[i].subData = (UInt64*) buffer->resource->GetGPUVirtualAddress();
            break;
        }
        }
    }
}

Void GraphicsModule::SetTexture(ShaderArguments* target, const Char* name, const Texture* texture)
{
    ASSERT(texture != nullptr);
    auto pipeline = (ShaderPipeline*) target->pipeline;
    auto& rootParameters = pipeline->rootParameters;
    auto& rootArguments = target->rootArguments;
    for (UInt i = 0; i < rootParameters.size(); i++)
    {
        auto& rootParameter = rootParameters[i];
        auto& rootArgument = rootArguments[i];
        switch (rootParameter.type)
        {
        case RootParamterType::TableSRV:
        {
            for (UInt j = 0; j < rootParameter.supParameters.size(); j++)
            {
                // Find argument that we want to change
                auto& rootSubParameter = rootParameter.supParameters[j];
                if (strcmp(rootSubParameter.name, name) != 0)
                    continue;

                // Don't update if the argument didn't changed
                if (rootArgument.subData[j] == (UInt64) texture)
                    return;
                rootArgument.subData[j] = (UInt64) texture;

                // If draw was used before, we want to make changes in new heap, to aVoid race condition
                if (rootArgument.IsCurrentlyUsedByDraw)
                {
                    rootArgument.IsCurrentlyUsedByDraw = false;

                    // Mark this memory as we need to deallocate once the buffer is finished
                    srvHeapMemoryToFree.push_back(std::pair<UInt64, HeapMemory>(planner->GetRecordingBufferIndex(), rootArgument.memory));

                    rootArgument.memory = srvHeap->Allocate(rootArgument.memory.size);
                    auto memory = rootArgument.memory;
                    for (UInt k = 0; k < rootParameter.supParameters.size(); k++)
                    {
                        if (rootParameter.supParameters[i].isTexture)
                        {
                            auto resource = (Texture*) rootArgument.subData[i];
                            device->CopyDescriptorsSimple(1, srvHeap->GetCpuHandle(memory), srvCpuHeap->GetCpuHandle(resource->srvMemory), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                            memory.address++;
                        }
                        else
                        {

                        }
                    }
                }
                else
                {
                    HeapMemory memory(rootArgument.memory.address + j, 1);
                    device->CopyDescriptorsSimple(1, srvHeap->GetCpuHandle(memory), srvCpuHeap->GetCpuHandle(texture->srvMemory), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
                return;
            }
            break;
        }
        }
    }
}

Void GraphicsModule::SetFilter(ShaderArguments* target, const Char* name, const Filter* filter)
{
    auto pipeline = (ShaderPipeline*) target->pipeline;
    auto& rootParameters = pipeline->rootParameters;
    auto& rootArguments = target->rootArguments;
    for (UInt i = 0; i < rootParameters.size(); i++)
    {
        auto& rootParameter = rootParameters[i];
        auto& rootArgument = rootArguments[i];
        switch (rootParameter.type)
        {
        case RootParamterType::TableSamplers:
        {
            for (UInt j = 0; j < rootParameter.supParameters.size(); j++)
            {
                // Find argument that we want to change
                auto& rootSubParameter = rootParameter.supParameters[j];
                if (strcmp(rootSubParameter.name, name) != 0)
                    continue;

                // Don't update if the argument didn't changed
                if (rootArgument.subData[j] == (UInt64) filter)
                    return;
                rootArgument.subData[j] = (UInt64) filter;

                // If draw was used before, we want to make changes in new heap, to aVoid race condition
                if (rootArgument.IsCurrentlyUsedByDraw)
                {
                    rootArgument.IsCurrentlyUsedByDraw = false;

                    // Mark this memory as we need to deallocate once the buffer is finished
                    samplersHeapMemoryToFree.push_back(std::pair<UInt64, HeapMemory>(planner->GetRecordingBufferIndex(), rootArgument.memory));

                    rootArgument.memory = samplersHeap->Allocate(rootArgument.memory.size);
                    auto memory = rootArgument.memory;
                    for (UInt k = 0; k < rootParameter.supParameters.size(); k++)
                    {
                        device->CopyDescriptorsSimple(1, samplersHeap->GetCpuHandle(memory), samplersCpuHeap->GetCpuHandle(filter->srvMemory), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                        memory.address++;
                    }
                }
                else
                {
                    HeapMemory memory(rootArgument.memory.address + j, 1);
                    device->CopyDescriptorsSimple(1, samplersHeap->GetCpuHandle(memory), samplersCpuHeap->GetCpuHandle(filter->srvMemory), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
                }
                return;
            }
            break;
        }
        }
    }
}

Void GraphicsModule::SetColorAttachment(const ExecutionContext& context, RenderPass* target, UInt32 index, const ColorAttachment& attachment)
{
    target->colors[index] = attachment;

    auto texture = (Texture*) attachment.texture;
    if (texture != nullptr)
    {
        target->colorDescriptors[index] = rtvHeap->GetCpuHandle(texture->rtvMemory);
        ASSERT(texture->rtvMemory.size != 0);
    }

    target->colorDescriptorsCount = 0;
    for (int i = 0; i < COLOR_ATTACHMENT_MAX_COUNT; i++)
        if (target->colors[i].texture != nullptr)
            target->colorDescriptorsCount++;

    if (index == 0)
        SetViewport(context, target, target->viewport);
}

Void GraphicsModule::SetDepthAttachment(const ExecutionContext& context, RenderPass* target, const DepthAttachment& attachment)
{
    target->depth = attachment;

    auto texture = (Texture*) attachment.texture;
    if (texture != nullptr)
        target->depthDescriptor = rtvHeap->GetCpuHandle(texture->rtvMemory);
}

Void GraphicsModule::SetViewport(const ExecutionContext& context, RenderPass* target, const Viewport& viewport)
{
    if (target->colors[0].texture == nullptr)
        return;

    auto texture = target->colors[0].texture;
    target->viewport.rect = viewport.rect;

    Rectf rect(
        viewport.rect.x * texture->width,
        viewport.rect.y * texture->height,
        viewport.rect.width * texture->width,
        viewport.rect.height * texture->height);

    target->d12ScissorRect.left = (long) rect.x;
    target->d12ScissorRect.right = (long) rect.GetRight();
    target->d12ScissorRect.top = (long) rect.y;
    target->d12ScissorRect.bottom = (long) rect.GetTop();

    target->d12Viewport.MinDepth = 0;
    target->d12Viewport.MaxDepth = 1;
    target->d12Viewport.TopLeftX = rect.x;
    target->d12Viewport.TopLeftY = rect.y;
    target->d12Viewport.Width = rect.width;
    target->d12Viewport.Height = rect.height;
}

Void GraphicsModule::SetRenderPass(const ExecutionContext & context, const RenderPass * target)
{
    for (auto& color : target->colors)
    {
        auto texture = (Texture*) color.texture;
        if (texture != nullptr)
            SetTextureState(context, texture, D3D12_RESOURCE_STATE_RENDER_TARGET);
    }
    auto depthTexture = (Texture*) target->depth.texture;
    if (depthTexture != nullptr)
    {
        SetTextureState(context, depthTexture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }
    DescriptorHeap* heaps[Enum::ToUnderlying(HeapType::Count)] = {
        srvHeap,
        samplersHeap,
        nullptr,
        nullptr,
        nullptr};
    planner->RecSetHeap((const DescriptorHeap**) heaps);
    planner->RecSetRenderPass(target);
}

Void GraphicsModule::UpdateBuffer(Buffer* target, UInt32 targetOffset, Range<UInt8> data)
{
    planner->RecUpdateBuffer(target, targetOffset, data);
}

inline Void GraphicsModule::Draw(const ExecutionContext& context, const DrawDesc& target)
{
    auto pipeline = (ShaderPipeline*) target.pipeline;
    auto arguments = (ShaderArguments*) target.properties;
    auto& rootParameters = pipeline->rootParameters;
    auto& rootArguments = arguments->rootArguments;
    for (UInt i = 0; i < rootParameters.size(); i++)
    {
        auto& rootParameter = rootParameters[i];
        switch (rootParameter.type)
        {
        case RootParamterType::TableSRV:
        {
            auto& rootArgument = rootArguments[i];
            for (UInt j = 0; j < rootParameter.supParameters.size(); j++)
            {
                if (rootParameter.supParameters[j].isTexture)
                {
                    auto resource = (Texture*) rootArguments[i].subData[j];
                    rootArguments[i].IsCurrentlyUsedByDraw = true;
                    SetTextureState(context, resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
                }
                else
                {
                    auto resource = (Buffer*) rootArguments[i].subData[j];
                    rootArguments[i].IsCurrentlyUsedByDraw = true;
                    SetBufferState(context, resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
                }
            }
            break;
        }
        case RootParamterType::TableSamplers:
        {
            auto& rootArgument = rootArguments[i];
            rootArgument.IsCurrentlyUsedByDraw = true;
            break;
        }
        }
    }
    planner->RecDraw(target);
}

Void GraphicsModule::SetName(ID3D12Object* object, const wchar_t* format, ...)
{
    wchar_t name[128];
    va_list ap;
    va_start(ap, format);
    ASSERT(vswprintf_s(name, 128, format, ap) > 0);
    object->SetName(name);
}

Void GraphicsModule::BlitCopy(const ExecutionContext& context, Texture* src, Texture* dest)
{
    // TODO: Make it cleaner, maybe move blit to abstraction if possible
    if (blitCopy == nullptr)
    {
        blitCopy = new BlitCopyDesc();
        InitializeBlitCopy(blitCopy);
    }

    SetColorAttachment(context, blitCopy->renderPass, 0, ColorAttachment(dest, StoreAction::Store, LoadAction::DontCare));
    SetRenderPass(context, blitCopy->renderPass);

    SetTexture((ShaderArguments*) blitCopy->properties, "_mainTex", src);
    Draw(context, *blitCopy);
}

Void GraphicsModule::Present(const ExecutionContext& context, SwapChain* swapChain, Texture* offscreen)
{
    planner->RecRequestSplit();

    // Blit ofscreen buffer to swapchain backbuffer
    auto bacBuffer = swapChain->GetBacBuffer();
    BlitCopy(context, offscreen, bacBuffer);

    ASSERT(swapChain->bacBufferIndex == swapChain->IDXGISwapChain3->GetCurrentBackBufferIndex());

    // Present backbuffer on screen
    SetTextureState(context, bacBuffer, D3D12_RESOURCE_STATE_PRESENT);
    planner->RecPresent(swapChain);

    swapChain->bacBufferIndex = (swapChain->bacBufferIndex + 1) % swapChain->bacBufferCount;
}

Bool GraphicsModule::Initialize()
{
    UINT dxgiFactoryFlags = 0;
#if defined(ENABLED_D12_DEBUG_LAYER)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ID3D12Debug* debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
        debugController->Release();
    }
#endif

    ASSERT_SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    //IDXGIAdapter* warpAdapter;
    //ASSERT_SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

    auto featureLevel = D3D_FEATURE_LEVEL_11_1;
    auto result = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&device));
    if (FAILED(result))
        return false;

    // This is the highest version the sample supports. If ChecFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
    rootSignatureFeatures.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureFeatures, sizeof(rootSignatureFeatures))))
        rootSignatureFeatures.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

    srvHeap = new DescriptorHeap(device, HeapType::SRVs, 128);
    rtvHeap = new DescriptorHeap(device, HeapType::RTVs, 128);
    samplersHeap = new DescriptorHeap(device, HeapType::Samplers, 128);
    srvCpuHeap = new DescriptorHeap(device, HeapType::SRVsCPU, 128);
    samplersCpuHeap = new DescriptorHeap(device, HeapType::SamplersCPU, 128);

    bufferUploadHeap = new BufferHeap(device, 1 << 20, 256, D3D12_HEAP_TYPE_UPLOAD);
    bufferDefaultHeap = new BufferHeap(device, 1 << 12, 256, D3D12_HEAP_TYPE_DEFAULT);

    return true;
}

Void GraphicsModule::InitializeSwapCain(SwapChain* swapChain)
{
    auto view = swapChain->view;
    auto bacBufferCount = swapChain->bacBufferCount;

    swapChain->width = view->width;
    swapChain->height = view->height;

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = bacBufferCount;
    swapChainDesc.Width = swapChain->width;
    swapChainDesc.Height = swapChain->height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    auto winView = (const View*) view;
    auto windowHandle = winView->windowHandle;
    IDXGISwapChain1* swapChain1;
    ASSERT_SUCCEEDED(factory->CreateSwapChainForHwnd(
        planner->GetDirectQueue(),        // Swap chain needs the queue so that it can force a flush on it.
        windowHandle,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1
    ));

    resourceCounter++;

    // This sample does not support fullscreen transitions.
    ASSERT_SUCCEEDED(factory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER));

    ASSERT_SUCCEEDED(swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain->IDXGISwapChain3)));
    auto d12SwapChain = swapChain->IDXGISwapChain3;
    swapChain->bacBufferIndex = d12SwapChain->GetCurrentBackBufferIndex();

    auto memory = rtvHeap->Allocate(bacBufferCount);

    // Create backbuffers
    swapChain->bacBuffers = new Texture*[bacBufferCount];
    for (UInt32 i = 0; i < bacBufferCount; i++)
    {
        auto bacBufferMemory = HeapMemory(memory.address + i, 1);

        ID3D12Resource* bacBufferResource;
        ASSERT_SUCCEEDED(d12SwapChain->GetBuffer(i, IID_PPV_ARGS(&bacBufferResource)));
        device->CreateRenderTargetView(bacBufferResource, nullptr, rtvHeap->GetCpuHandle(bacBufferMemory));

        auto bacBuffer = new Texture(swapChain->width, swapChain->height);
        bacBuffer->usage = TextureUsageFlags::Render;
        bacBuffer->rtvMemory = bacBufferMemory;
        bacBuffer->resource = bacBufferResource;
        bacBuffer->currentState = D3D12_RESOURCE_STATE_PRESENT;
        swapChain->bacBuffers[i] = bacBuffer;
        SetName(bacBufferResource, L"BacBuffer %d (SwapChain %d)", i, resourceCounter);
    }
}

Void GraphicsModule::InitializeTexture(Texture* texture)
{
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width = texture->width;
    textureDesc.Height = texture->height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    if (Enum::Contains(texture->usage, TextureUsageFlags::Render))
        textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    D3D12_CLEAR_VALUE clearValue;
    Bool useClearVlaue = false;
    
    // If we match the clear values with render pass, we win performance
    if (Enum::Contains(texture->usage, TextureUsageFlags::Render))
    {
        clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        clearValue.Color[0] = 0.19f;
        clearValue.Color[1] = 0.3f;
        clearValue.Color[2] = 0.47f;
        clearValue.Color[3] = 0;
        useClearVlaue = true;
    }

    ASSERT_SUCCEEDED(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        texture->currentState,
        useClearVlaue ? &clearValue : nullptr,
        IID_PPV_ARGS(&texture->resource)));

    // Create SRV
    if (Enum::Contains(texture->usage, TextureUsageFlags::Shader))
    {
        texture->srvMemory = srvCpuHeap->Allocate(1);
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        device->CreateShaderResourceView(texture->resource, &srvDesc, srvCpuHeap->GetCpuHandle(texture->srvMemory));
    }

    // Create RTV
    if (Enum::Contains(texture->usage, TextureUsageFlags::Render))
    {
        texture->rtvMemory = rtvHeap->Allocate(1);
        device->CreateRenderTargetView(texture->resource, nullptr, rtvHeap->GetCpuHandle(texture->rtvMemory));
    }
}

inline Void GraphicsModule::InitializeFilter(Filter* filter)
{
    filter->srvMemory = samplersHeap->Allocate(1);
    // TODO: Maybe lets keep desc in texture?
    D3D12_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    samplerDesc.BorderColor[0] = 1.0f;
    samplerDesc.BorderColor[1] = 1.0f;
    samplerDesc.BorderColor[2] = 1.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    device->CreateSampler(&samplerDesc, samplersHeap->GetCpuHandle(filter->srvMemory));
}

Void GraphicsModule::InitializeRenderPass(RenderPass* renderPass)
{
}

Void GraphicsModule::InitializeBuffer(Buffer* target)
{
    /*ASSERT_SUCCEEDED(device->CreateCommittedResource(
    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
    D3D12_HEAP_FLAG_NONE,
    &CD3DX12_RESOURCE_DESC::Buffer(target->data.size),
    target->currentState,
    nullptr,
    IID_PPV_ARGS(&target->resource)));
    target->cachedResourceGpuVirtualAddress = target->resource->GetGPUVirtualAddress();*/

    if ((target->usage & BufferUsageFlags::CpuToGpu) != BufferUsageFlags::None)
    {
        target->memory = bufferUploadHeap->Allocate(target->GetSize());
        //target->currentState = D3D12_RESOURCE_STATE_GENERIC_READ;
        target->state = bufferUploadHeap->GetState(target->memory);
        target->SetState(D3D12_RESOURCE_STATE_GENERIC_READ);
        target->cachedResourceGpuVirtualAddress = bufferUploadHeap->GetVirtualAddress(target->memory);
        target->resource = bufferUploadHeap->GetResource(target->memory);
        target->resourceMappedPointer = bufferUploadHeap->GetResourceMappedPointer(target->memory);
        target->resourceOffset = bufferUploadHeap->GetResourceOffset(target->memory);
    }

    if ((target->usage & BufferUsageFlags::GpuOnly) != BufferUsageFlags::None)
    {
        target->memory = bufferDefaultHeap->Allocate(target->GetSize());
        //target->currentState = D3D12_RESOURCE_STATE_GENERIC_READ;
        target->state = bufferDefaultHeap->GetState(target->memory);
        target->SetState(D3D12_RESOURCE_STATE_GENERIC_READ);
        target->cachedResourceGpuVirtualAddress = bufferDefaultHeap->GetVirtualAddress(target->memory);
        target->resource = bufferDefaultHeap->GetResource(target->memory);
        target->resourceOffset = bufferDefaultHeap->GetResourceOffset(target->memory);
    }


}

Void GraphicsModule::CompilePipeline(ShaderPipeline* pipeline)
{
    // TODO: Make it universal
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(ENABLED_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0; 
#endif

    ComPtr<ID3DBlob> error;
    D3DCompile(pipeline->source, pipeline->sourceSize, nullptr, nullptr, nullptr, "VertMain", "vs_5_0", compileFlags, 0, &vertexShader, &error);
    if (error.Get() != NULL)
        ERROR((const Char*) error.Get()->GetBufferPointer());

    D3DCompile(pipeline->source, pipeline->sourceSize, nullptr, nullptr, nullptr, "FragMain", "ps_5_0", compileFlags, 0, &pixelShader, &error);
    if (error.Get() != NULL)
        ERROR((const Char*) error.Get()->GetBufferPointer());

    pipeline->programs[Enum::ToUnderlying(ShaderProgramType::Vertex)] = ShaderProgram(new UInt8[vertexShader.Get()->GetBufferSize()], vertexShader.Get()->GetBufferSize());
    pipeline->programs[Enum::ToUnderlying(ShaderProgramType::Fragment)] = ShaderProgram(new UInt8[pixelShader.Get()->GetBufferSize()], pixelShader.Get()->GetBufferSize());

    memcpy((Void*) pipeline->programs[Enum::ToUnderlying(ShaderProgramType::Vertex)].code, (const UInt8*) vertexShader.Get()->GetBufferPointer(), vertexShader.Get()->GetBufferSize());
    memcpy((Void*) pipeline->programs[Enum::ToUnderlying(ShaderProgramType::Fragment)].code, (const UInt8*) pixelShader.Get()->GetBufferPointer(), pixelShader.Get()->GetBufferSize());
}

Void GraphicsModule::InitializePipeline(ShaderPipeline* pipeline)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

    // Check if we need to compile shader first
    if (pipeline->sourceSize != 0 && pipeline->source != nullptr)
    {
        CompilePipeline(pipeline);
    }

    // Create shaders
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> fragmentShader;
    {
        if (pipeline->programs[Enum::ToUnderlying(ShaderProgramType::Vertex)].available)
        {
            auto shaderProgram = pipeline->programs[Enum::ToUnderlying(ShaderProgramType::Vertex)];
            ASSERT_SUCCEEDED(D3DCreateBlob(shaderProgram.size, &vertexShader));
            memcpy(vertexShader.Get()->GetBufferPointer(), shaderProgram.code, shaderProgram.size);
            psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        }

        if (pipeline->programs[Enum::ToUnderlying(ShaderProgramType::Fragment)].available)
        {
            auto shaderProgram = pipeline->programs[Enum::ToUnderlying(ShaderProgramType::Fragment)];
            ASSERT_SUCCEEDED(D3DCreateBlob(shaderProgram.size, &fragmentShader));
            memcpy(fragmentShader.Get()->GetBufferPointer(), shaderProgram.code, shaderProgram.size);
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(fragmentShader.Get());
        }
    }

    // Crete signature
    {
        auto& rootParameters = pipeline->rootParameters;
        auto& parameters = pipeline->parameters;
        for (auto& parameter : parameters)
        {
            switch (parameter.type)
            {
            case ShaderParameterType::ConstantBuffer:
            {
                rootParameters.push_back(RootParamter::AsConstantBuffer(parameter.name.c_str()));
                break;
            }
            case ShaderParameterType::Buffer:
            {
                auto& rootParamter = pipeline->FindRootParameter(RootParamterType::TableSRV);
                rootParamter.supParameters.push_back(RootSubParamter(parameter.name.c_str(), D3D12_DESCRIPTOR_RANGE_TYPE_SRV));
                break;
            }
            case ShaderParameterType::Texture:
            {
                auto& rootParamter = pipeline->FindRootParameter(RootParamterType::TableSRV);
                rootParamter.supParameters.push_back(RootSubParamter(parameter.name.c_str(), D3D12_DESCRIPTOR_RANGE_TYPE_SRV, true));
                break;
            }
            case ShaderParameterType::Sampler:
            {
                auto& rootParamter = pipeline->FindRootParameter(RootParamterType::TableSamplers);
                rootParamter.supParameters.push_back(RootSubParamter(parameter.name.c_str(), D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER));
                break;
            }
            }
        }
        auto d12RootParameters = new CD3DX12_ROOT_PARAMETER1[rootParameters.size()];
        for (UINT i = 0; i < (UINT)rootParameters.size(); i++)
        {
            auto& rootParameter = rootParameters[i];
            switch (rootParameter.type)
            {
            case RootParamterType::ConstantBuffer:
            {
                d12RootParameters[i].InitAsConstantBufferView(i, 0U, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_ALL);
                break;
            }
            case RootParamterType::TableSRV:
            {
                UInt32 srvCount = 0;
                rootParameter.GetCounts(&srvCount, nullptr);

                CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
                ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, srvCount, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

                d12RootParameters[i].InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_ALL);
                break;
            }
            case RootParamterType::TableSamplers:
            {
                UInt32 samplersCount = 0;
                rootParameter.GetCounts(nullptr, &samplersCount);

                CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
                ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, samplersCount, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

                d12RootParameters[i].InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_ALL);
                break;
            }
            }
        }
        
        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1((UINT) rootParameters.size(), d12RootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, rootSignatureFeatures.HighestVersion, &signature, &error);
        if (error.Get() != NULL)
            ERROR((const Char*)error.Get()->GetBufferPointer());

        ASSERT_SUCCEEDED(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pipeline->rootSignature)));
        psoDesc.pRootSignature = pipeline->rootSignature;

        delete d12RootParameters;
    }

    // Create vertex layout
    {
        auto vertexLayout = &pipeline->vertexLayout;
        auto vertexAttributeCount = vertexLayout->attributes.size();
        auto vertexAtrrbiutesSize = 0;
        ASSERT(vertexLayout != nullptr);
        auto inputElementDescs = new D3D12_INPUT_ELEMENT_DESC[vertexAttributeCount];
        for (int i = 0; i < vertexAttributeCount; i++)
        {
            auto& vertexAttribute = vertexLayout->attributes[i];
            auto& input = inputElementDescs[i];
            input.SemanticName = Convert(vertexAttribute.type);
            input.SemanticIndex = 0;
            input.Format = Convert(vertexAttribute.format);
            input.InputSlot = 0;
            input.AlignedByteOffset = vertexAtrrbiutesSize;
            input.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            input.InstanceDataStepRate = 0;
            vertexAtrrbiutesSize += GetSize(vertexAttribute.format);
        }
        pipeline->vertexBuffer.StrideInBytes = vertexAtrrbiutesSize;
        psoDesc.InputLayout = { inputElementDescs, (UINT) vertexAttributeCount };
    }

    // Set defaults
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    ASSERT_SUCCEEDED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipeline->pipelineState)));
}

Void GraphicsModule::InitializeProperties(ShaderArguments* target)
{
    auto pipeline = (ShaderPipeline*) target->pipeline;
    auto& rootParameters = pipeline->rootParameters;
    auto& rootArguments = target->rootArguments;
    for (auto& rootParameter : rootParameters)
    {
        switch (rootParameter.type)
        {
        case RootParamterType::TableSRV:
        {
            auto memory = srvHeap->Allocate(rootParameter.supParameters.size());
            rootArguments.push_back(RootArgument(memory));
            break;
        }
        case RootParamterType::TableSamplers:
        {
            auto memory = samplersHeap->Allocate(rootParameter.supParameters.size());
            rootArguments.push_back(RootArgument(memory));
            break;
        }
        case RootParamterType::ConstantBuffer:
        {
            rootArguments.push_back(RootArgument(0));
            break;
        }
        }
    }
}

Void GraphicsModule::InitializeBlitCopy(BlitCopyDesc* target)
{
    auto source =
        R"(

struct AppData
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertData
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D _mainTex : register(t0);
SamplerState _mainTexSampler : register(s0);

VertData VertMain(AppData i)
{
    VertData o;
    o.position = float4(i.position, 1);
    o.uv = i.uv;
    return o;
}

float4 FragMain(VertData i) : SV_TARGET
{
    return _mainTex.Sample(_mainTexSampler, float2(i.uv.x, 1 - i.uv.y));
}
            )";

    VertexLayout vertexLayout;
    vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeType::Position, ColorFormat::RGB24));
    vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeType::TexCoord0, ColorFormat::RG16));

    auto shaderDesc = new ShaderPipelineDesc();
    shaderDesc->name = "Test";
    shaderDesc->source = (const UInt8*) source;
    shaderDesc->sourceSize = strlen(source);
    shaderDesc->states.zTest = ZTest::LEqual;
    shaderDesc->states.zWrite = ZWrite::On;
    shaderDesc->varation = 0;
    shaderDesc->vertexLayout = vertexLayout;
    shaderDesc->parameters.push_back(ShaderParameter("_mainTex", ShaderParameterType::Texture));
    shaderDesc->parameters.push_back(ShaderParameter("_mainTexSampler", ShaderParameterType::Sampler));
    target->pipeline = new ShaderPipeline(shaderDesc);
    InitializePipeline((ShaderPipeline*)target->pipeline);

    target->properties = new ShaderArguments(target->pipeline);
    InitializeProperties((ShaderArguments*) target->properties);

    static Float vertices[] = {
        -1, -1, 0,    0, 0,
        -1, 1, 0,    0, 1,
        1, 1, 0,    1, 1,

        -1, -1, 0,    0, 0,
        1, 1, 0,    1, 1,
        1, -1, 0,    1, 0,
    };
    target->vertexBuffer = new Buffer(sizeof(vertices));
    InitializeBuffer((Buffer*) target->vertexBuffer);
    UpdateBuffer((Buffer*) target->vertexBuffer, 0, Range<UInt8>((const UInt8*) vertices, sizeof(vertices)));

    target->offset = 0;
    target->size = 3*2;

    target->renderPass = new RenderPass();
    InitializeRenderPass(target->renderPass);

    target->filter = new Filter();
    InitializeFilter(target->filter);
    SetFilter((ShaderArguments*) target->properties, "_mainTexSampler", target->filter);
}

DXGI_FORMAT GraphicsModule::Convert(ColorFormat format)
{
    // TODO: Table lookup
    switch (format)
    {
    case ColorFormat::RGBA32:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case ColorFormat::RGB24:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case ColorFormat::RG16:
        return DXGI_FORMAT_R32G32_FLOAT;
    }
    
    ERROR("Format is not convertable");
    return (DXGI_FORMAT) -1;
}

const Char* GraphicsModule::Convert(VertexAttributeType type)
{
    // TODO: Table lookup
    switch (type)
    {
    case VertexAttributeType::Position:
        return "POSITION";
    case VertexAttributeType::TexCoord0:
        return "TEXCOORD";
    }

    ERROR("Type is not convertable");
    return nullptr;
}

UInt32 GraphicsModule::GetSize(ColorFormat format)
{
    switch (format)
    {
    case ColorFormat::RGBA32:
        return sizeof(Float) * 4;
    case ColorFormat::RGB24:
        return sizeof(Float) * 3;
    case ColorFormat::RG16:
        return sizeof(Float) * 2;
    }

    ERROR("Format is not convertable");
    return -1;
}