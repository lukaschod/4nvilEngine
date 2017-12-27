#include <Windows\Graphics\D12\D12GraphicsModule.h>
#include <Windows\Graphics\D12\D12GraphicsPlannerModule.h>
#include <Windows\Graphics\D12\D12GraphicsExecuterModule.h>

D12GraphicsModule::D12GraphicsModule(uint32_t bufferCount, uint32_t bufferIndexStep)
	: IGraphicsModule(bufferCount, bufferIndexStep)
	, device(nullptr)
	, factory(nullptr)
	, resourceCounter(0)
{}

void D12GraphicsModule::Execute(const ExecutionContext& context)
{
	// Deallocate all memory that is not used by GPU
	auto completedBufferIndex = planner->GetCompletedBufferIndex();
	for (size_t i = srvHeapMemoryToFree.size(); i-->0 ;)
	{
		auto memory = srvHeapMemoryToFree.back();
		if (memory.first > completedBufferIndex)
			continue;
		srvHeapMemoryToFree.pop_back();
		srvHeap->Deallocate(memory.second);
	}
	for (size_t i = samplersHeapMemoryToFree.size(); i-->0;)
	{
		auto memory = samplersHeapMemoryToFree.back();
		if (memory.first > completedBufferIndex)
			continue;
		samplersHeapMemoryToFree.pop_back();
		samplersHeap->Deallocate(memory.second);
	}

	planner->Reset();
	CmdModule::Execute(context);
}

void D12GraphicsModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	Initialize();
	moduleManager->AddModule(new D12GraphicsPlannerModule(device));
	moduleManager->AddModule(new D12GraphicsExecuterModule(4));
	planner = ExecuteBefore<D12GraphicsPlannerModule>(moduleManager);
	memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
	memoryModule->SetAllocator(1, new FixedBlockHeap(sizeof(D12ShaderArguments)));
	memoryModule->SetAllocator(2, new FixedBlockHeap(sizeof(D12Buffer)));
}

const IBuffer* D12GraphicsModule::AllocateBuffer(size_t size)
{
	return memoryModule->New<D12Buffer>(2, size);
}

const ITexture* D12GraphicsModule::AllocateTexture(uint32_t width, uint32_t height)
{
	return new D12Texture(width, height);
}

const ISwapChain* D12GraphicsModule::AllocateSwapChain(const IView * view)
{
	return new D12SwapChain(view);
}

SERIALIZE_METHOD_CREATEGEN_ARG1(D12GraphicsModule, IFilter, D12Filter, const FilterOptions&);
SERIALIZE_METHOD_CREATEGEN(D12GraphicsModule, IRenderPass, D12RenderPass);
SERIALIZE_METHOD_ARG3(D12GraphicsModule, SetColorAttachment, const IRenderPass*, uint32_t, const ColorAttachment&);
SERIALIZE_METHOD_ARG2(D12GraphicsModule, SetDepthAttachment, const IRenderPass*, const DepthAttachment&);
SERIALIZE_METHOD_ARG2(D12GraphicsModule, SetViewport, const IRenderPass*, const Viewport&);
SERIALIZE_METHOD_ARG1(D12GraphicsModule, SetRenderPass, const IRenderPass*);
SERIALIZE_METHOD_CREATEGEN_ARG1(D12GraphicsModule, IShaderPipeline, D12ShaderPipeline, const ShaderPipelineDesc*);
SERIALIZE_METHOD_ARG3(D12GraphicsModule, SetBuffer, const IShaderArguments*, const char*, const IBuffer*);
SERIALIZE_METHOD_ARG3(D12GraphicsModule, SetTexture, const IShaderArguments*, const char*, const ITexture*);
SERIALIZE_METHOD_ARG3(D12GraphicsModule, SetFilter, const IShaderArguments*, const char*, const IFilter*);
SERIALIZE_METHOD_ARG2(D12GraphicsModule, Present, const ISwapChain*, const ITexture*);
SERIALIZE_METHOD_ARG2(D12GraphicsModule, FinalBlit, const ISwapChain*, const ITexture*);
SERIALIZE_METHOD_ARG3(D12GraphicsModule, UpdateBuffer, const IBuffer*, void*, size_t);
SERIALIZE_METHOD_ARG1(D12GraphicsModule, PushDebug, const char*);
SERIALIZE_METHOD(D12GraphicsModule, PopDebug);
SERIALIZE_METHOD_ARG1(D12GraphicsModule, Draw, const DrawDesc&);

DECLARE_COMMAND_CODE(CreateIShaderArguments);
const IShaderArguments* D12GraphicsModule::RecCreateIShaderArguments(const ExecutionContext& context, const IShaderPipeline* pipeline)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = memoryModule->New<D12ShaderArguments>(1, pipeline);
	stream.Write(CommandCodeCreateIShaderArguments);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}
//SERIALIZE_METHOD_CREATEGEN_ARG1(D12GraphicsModule, IShaderArguments, D12ShaderArguments, const IShaderPipeline*);

DECLARE_COMMAND_CODE(CreateIBuffer);
const IBuffer* D12GraphicsModule::RecCreateIBuffer(const ExecutionContext& context, size_t size, const IBuffer* gfxBuffer)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = gfxBuffer == nullptr ? AllocateBuffer(size) : gfxBuffer;
	stream.Write(CommandCodeCreateIBuffer);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}
//SERIALIZE_METHOD_CREATEGEN_ARG1(D12GraphicsModule, IBuffer, D12Buffer, size_t);

DECLARE_COMMAND_CODE(CreateISwapChain);
const ISwapChain* D12GraphicsModule::RecCreateISwapChain(const ExecutionContext& context, const IView* view, const ISwapChain* swapChain)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = swapChain == nullptr ? AllocateSwapChain(view) : swapChain;
	stream.Write(CommandCodeCreateISwapChain);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}
//SERIALIZE_METHOD_CREATEGEN_ARG1(D12GraphicsModule, ISwapChain, D12SwapChain, const IView*);

DECLARE_COMMAND_CODE(CreateITexture);
const ITexture* D12GraphicsModule::RecCreateITexture(const ExecutionContext& context, uint32_t width, uint32_t height, const ITexture* texture)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = texture == nullptr ? AllocateTexture(width, height) : texture;
	stream.Write(CommandCodeCreateITexture);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}
//SERIALIZE_METHOD_CREATEGEN_ARG2(D12GraphicsModule, ITexture, D12Texture, uint32_t, uint32_t);

bool D12GraphicsModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateITexture, D12Texture*, target);
		InitializeTexture(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(CreateIFilter, D12Filter*, target);
		InitializeFilter(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(CreateIRenderPass, D12RenderPass*, target);
		InitializeRenderPass(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetColorAttachment, D12RenderPass*, target, uint32_t, index, ColorAttachment, attachment);
		SetColorAttachment(context, target, index, attachment);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetDepthAttachment, D12RenderPass*, target, DepthAttachment, attachment);
		SetDepthAttachment(context, target, attachment);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetViewport, D12RenderPass*, target, Viewport, viewport);
		SetViewport(context, target, viewport);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(SetRenderPass, D12RenderPass*, target);
		SetRenderPass(context, target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(CreateIShaderPipeline, D12ShaderPipeline*, target);
		InitializePipeline(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(CreateIShaderArguments, D12ShaderArguments*, target);
		InitializeProperties(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetTexture, D12ShaderArguments*, target, const char*, name, D12Texture*, texture);
		SetTexture(target, name, texture);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetFilter, D12ShaderArguments*, target, const char*, name, D12Filter*, filter);
		SetFilter(target, name, filter);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetBuffer, D12ShaderArguments*, target, const char*, name, D12Buffer*, buffer);
		SetBuffer(target, name, buffer);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(CreateISwapChain, D12SwapChain*, target);
		InitializeSwapCain(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(Present, D12SwapChain*, target, D12Texture*, texture);
		planner->RecPresent(target);
		target->bacBufferIndex = (target->bacBufferIndex + 1) % target->bacBufferCount;
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(FinalBlit, D12SwapChain*, target, D12Texture*, texture);
		// Blit ofscreen buffer to swapchain backbuffer
		auto bacBuffer = target->GetBacBuffer();
		BlitCopy(context, texture, bacBuffer);

		ASSERT(target->bacBufferIndex == target->IDXGISwapChain3->GetCurrentBackBufferIndex());

		SetTextureState(context, bacBuffer, D3D12_RESOURCE_STATE_PRESENT);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(CreateIBuffer, D12Buffer*, target);
		InitializeBuffer(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(UpdateBuffer, D12Buffer*, target, void*, data, size_t, size);
		UpdateBuffer(target, 0, Range<uint8_t>((const uint8_t*)data, size));
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(PushDebug, const char*, name);
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

void D12GraphicsModule::SetTextureState(const ExecutionContext& context, D12Texture* target, D3D12_RESOURCE_STATES state)
{
	if (target->currentState == state)
		return;

	ASSERT(target->resource != nullptr);
	planner->RecSetTextureState(target, target->currentState, state);
	target->currentState = state;
}

void D12GraphicsModule::SetBufferState(const ExecutionContext& context, D12Buffer* target, D3D12_RESOURCE_STATES state)
{
	if (target->currentState == state)
		return;

	ASSERT(target->resource != nullptr);
	planner->RecSetBufferState(target, target->currentState, state);
	target->currentState = state;
}

void D12GraphicsModule::SetBuffer(D12ShaderArguments* target, const char* name, const D12Buffer* buffer)
{
	ASSERT(buffer != nullptr);
	auto pipeline = (D12ShaderPipeline*) target->pipeline;
	auto& rootParameters = pipeline->rootParameters;
	auto& rootArguments = target->rootArguments;
	ASSERT(rootParameters.size() == rootArguments.size());
	for (size_t i = 0; i < rootParameters.size(); i++)
	{
		auto& rootParameter = rootParameters[i];
		switch (rootParameter.type)
		{
		/*case D12RootParamterTypeTableSRV:
		{
			for (size_t j = 0; j < rootParameter.supParameters.size(); j++)
			{
				auto& rootSubParameter = rootParameter.supParameters[j];
				if (strcmp(rootSubParameter.name, name) != 0)
					continue;

				if (rootArguments[i].subData[j] == (uint64_t) buffer)
					continue;
				rootArguments[i].subData[j] = (uint64_t) buffer;

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

		case D12RootParamterTypeConstantBuffer:
		{
			auto& rootSubParameter = rootParameter.supParameters[0];
			if (strcmp(rootSubParameter.name, name) != 0)
				continue;

			// TODO: Check if we can really cache the gpu virtual address safely
			 rootArguments[i].subData = (uint64_t*) buffer->cachedResourceGpuVirtualAddress;
			// rootArguments[i].subData = (uint64_t*) buffer->resource->GetGPUVirtualAddress();
			break;
		}
		}
	}
}

void D12GraphicsModule::SetTexture(D12ShaderArguments* target, const char* name, const D12Texture* texture)
{
	ASSERT(texture != nullptr);
	auto pipeline = (D12ShaderPipeline*) target->pipeline;
	auto& rootParameters = pipeline->rootParameters;
	auto& rootArguments = target->rootArguments;
	for (size_t i = 0; i < rootParameters.size(); i++)
	{
		auto& rootParameter = rootParameters[i];
		auto& rootArgument = rootArguments[i];
		switch (rootParameter.type)
		{
		case D12RootParamterTypeTableSRV:
		{
			for (size_t j = 0; j < rootParameter.supParameters.size(); j++)
			{
				// Find argument that we want to change
				auto& rootSubParameter = rootParameter.supParameters[j];
				if (strcmp(rootSubParameter.name, name) != 0)
					continue;

				// Don't update if the argument didn't changed
				if (rootArgument.subData[j] == (uint64_t) texture)
					return;
				rootArgument.subData[j] = (uint64_t) texture;

				// If draw was used before, we want to make changes in new heap, to avoid race condition
				if (rootArgument.IsCurrentlyUsedByDraw)
				{
					rootArgument.IsCurrentlyUsedByDraw = false;

					// Mark this memory as we need to deallocate once the buffer is finished
					srvHeapMemoryToFree.push_back(std::pair<uint64_t, D12HeapMemory>(planner->GetRecordingBufferIndex(), rootArgument.memory));

					rootArgument.memory = srvHeap->Allocate(rootArgument.memory.size);
					auto memory = rootArgument.memory;
					for (size_t k = 0; k < rootParameter.supParameters.size(); k++)
					{
						if (rootParameter.supParameters[i].isTexture)
						{
							auto resource = (D12Texture*) rootArgument.subData[i];
							device->CopyDescriptorsSimple(1, srvHeap->GetCpuHandle(memory), srvCpuHeap->GetCpuHandle(resource->srvMemory), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
							memory.pointer++;
						}
						else
						{

						}
					}
				}
				else
				{
					D12HeapMemory memory(rootArgument.memory.pointer + j, 1);
					device->CopyDescriptorsSimple(1, srvHeap->GetCpuHandle(memory), srvCpuHeap->GetCpuHandle(texture->srvMemory), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				return;
			}
			break;
		}
		}
	}
}

void D12GraphicsModule::SetFilter(D12ShaderArguments* target, const char* name, const D12Filter* filter)
{
	auto pipeline = (D12ShaderPipeline*) target->pipeline;
	auto& rootParameters = pipeline->rootParameters;
	auto& rootArguments = target->rootArguments;
	for (size_t i = 0; i < rootParameters.size(); i++)
	{
		auto& rootParameter = rootParameters[i];
		auto& rootArgument = rootArguments[i];
		switch (rootParameter.type)
		{
		case D12RootParamterTypeTableSamplers:
		{
			for (size_t j = 0; j < rootParameter.supParameters.size(); j++)
			{
				// Find argument that we want to change
				auto& rootSubParameter = rootParameter.supParameters[j];
				if (strcmp(rootSubParameter.name, name) != 0)
					continue;

				// Don't update if the argument didn't changed
				if (rootArgument.subData[j] == (uint64_t) filter)
					return;
				rootArgument.subData[j] = (uint64_t) filter;

				// If draw was used before, we want to make changes in new heap, to avoid race condition
				if (rootArgument.IsCurrentlyUsedByDraw)
				{
					rootArgument.IsCurrentlyUsedByDraw = false;

					// Mark this memory as we need to deallocate once the buffer is finished
					samplersHeapMemoryToFree.push_back(std::pair<uint64_t, D12HeapMemory>(planner->GetRecordingBufferIndex(), rootArgument.memory));

					rootArgument.memory = samplersHeap->Allocate(rootArgument.memory.size);
					auto memory = rootArgument.memory;
					for (size_t k = 0; k < rootParameter.supParameters.size(); k++)
					{
						device->CopyDescriptorsSimple(1, samplersHeap->GetCpuHandle(memory), samplersCpuHeap->GetCpuHandle(filter->srvMemory), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
						memory.pointer++;
					}
				}
				else
				{
					D12HeapMemory memory(rootArgument.memory.pointer + j, 1);
					device->CopyDescriptorsSimple(1, samplersHeap->GetCpuHandle(memory), samplersCpuHeap->GetCpuHandle(filter->srvMemory), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
				}
				return;
			}
			break;
		}
		}
	}
}

void D12GraphicsModule::SetColorAttachment(const ExecutionContext& context, D12RenderPass* target, uint32_t index, const ColorAttachment& attachment)
{
	target->colors[index] = attachment;

	auto texture = (D12Texture*) attachment.texture;
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

void D12GraphicsModule::SetDepthAttachment(const ExecutionContext& context, D12RenderPass* target, const DepthAttachment& attachment)
{
	target->depth = attachment;

	auto texture = (D12Texture*) attachment.texture;
	if (texture != nullptr)
		target->depthDescriptor = rtvHeap->GetCpuHandle(texture->rtvMemory);
}

void D12GraphicsModule::SetViewport(const ExecutionContext& context, D12RenderPass* target, const Viewport& viewport)
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

void D12GraphicsModule::SetRenderPass(const ExecutionContext & context, const D12RenderPass * target)
{
	for (auto& color : target->colors)
	{
		auto texture = (D12Texture*) color.texture;
		if (texture != nullptr)
			SetTextureState(context, texture, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
	auto depthTexture = (D12Texture*) target->depth.texture;
	if (depthTexture != nullptr)
	{
		SetTextureState(context, depthTexture, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}
	D12DescriptorHeap* heaps[D12HeapTypeCount] = {
		srvHeap,
		samplersHeap,
		nullptr,
		nullptr,
		nullptr};
	planner->RecSetHeap((const D12DescriptorHeap**) heaps);
	planner->RecSetRenderPass(target);
}

void D12GraphicsModule::UpdateBuffer(D12Buffer* target, uint32_t targetOffset, Range<uint8_t> data)
{
	planner->RecUpdateBuffer(target, targetOffset, data);
}

inline void D12GraphicsModule::Draw(const ExecutionContext& context, const DrawDesc& target)
{
	auto pipeline = (D12ShaderPipeline*) target.pipeline;
	auto arguments = (D12ShaderArguments*) target.properties;
	auto& rootParameters = pipeline->rootParameters;
	auto& rootArguments = arguments->rootArguments;
	for (size_t i = 0; i < rootParameters.size(); i++)
	{
		auto& rootParameter = rootParameters[i];
		switch (rootParameter.type)
		{
		case D12RootParamterTypeTableSRV:
		{
			auto& rootArgument = rootArguments[i];
			for (size_t j = 0; j < rootParameter.supParameters.size(); j++)
			{
				if (rootParameter.supParameters[j].isTexture)
				{
					auto resource = (D12Texture*) rootArguments[i].subData[j];
					rootArguments[i].IsCurrentlyUsedByDraw = true;
					SetTextureState(context, resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
				}
				else
				{
					auto resource = (D12Buffer*) rootArguments[i].subData[j];
					rootArguments[i].IsCurrentlyUsedByDraw = true;
					SetBufferState(context, resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
				}
			}
			break;
		}
		case D12RootParamterTypeTableSamplers:
		{
			auto& rootArgument = rootArguments[i];
			rootArgument.IsCurrentlyUsedByDraw = true;
			break;
		}
		}
	}
	planner->RecDraw(target);
}

void D12GraphicsModule::SetName(ID3D12Object* object, const wchar_t* format, ...)
{
	wchar_t name[128];
	va_list ap;
	va_start(ap, format);
	ASSERT(vswprintf_s(name, 128, format, ap) > 0);
	object->SetName(name);
}

void D12GraphicsModule::BlitCopy(const ExecutionContext& context, D12Texture* src, D12Texture* dest)
{
	// TODO: Make it cleaner, maybe move blit to abstraction if possible
	static D12BlitCopy* blitCopy = nullptr;
	if (blitCopy == nullptr)
	{
		blitCopy = new D12BlitCopy();
		InitializeBlitCopy(blitCopy);
	}

	SetColorAttachment(context, blitCopy->renderPass, 0, ColorAttachment(dest, StoreActionStore, LoadActionDontCare));
	SetRenderPass(context, blitCopy->renderPass);

	SetTexture((D12ShaderArguments*) blitCopy->properties, "_mainTex", src);
	Draw(context, *blitCopy);
}

void D12GraphicsModule::Present(const ExecutionContext& context, D12SwapChain* swapChain, D12Texture* offscreen)
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

bool D12GraphicsModule::Initialize()
{
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
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

	IDXGIAdapter* warpAdapter;
	ASSERT_SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

	auto featureLevel = D3D_FEATURE_LEVEL_12_1;
	auto result = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&device));
	if (FAILED(result))
		return false;

	// This is the highest version the sample supports. If ChecFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
	rootSignatureFeatures.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSignatureFeatures, sizeof(rootSignatureFeatures))))
		rootSignatureFeatures.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

	srvHeap = new D12DescriptorHeap(device, D12HeapTypeSRVs, 128);
	rtvHeap = new D12DescriptorHeap(device, D12HeapTypeRTVs, 128);
	samplersHeap = new D12DescriptorHeap(device, D12HeapTypeSamplers, 128);
	srvCpuHeap = new D12DescriptorHeap(device, D12HeapTypeSRVsCPU, 128);
	samplersCpuHeap = new D12DescriptorHeap(device, D12HeapTypeSamplersCPU, 128);

	bufferUploadHeap = new D12BufferHeap(device, 200*201*256);

	ID3D12CommandAllocator* alloc;
	ASSERT_SUCCEEDED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&alloc)));
	ID3D12GraphicsCommandList* cmd;
	ASSERT_SUCCEEDED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, alloc, nullptr, IID_PPV_ARGS(&cmd)));

	return true;
}

void D12GraphicsModule::InitializeSwapCain(D12SwapChain* swapChain)
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

	auto winView = (const WinView*) view;
	auto windowHandle = winView->windowHandle;
	IDXGISwapChain1* swapChain1;
	ASSERT_SUCCEEDED(factory->CreateSwapChainForHwnd(
		planner->GetDirectQueue(),		// Swap chain needs the queue so that it can force a flush on it.
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
	swapChain->bacBuffers = new D12Texture*[bacBufferCount];
	for (uint32_t i = 0; i < bacBufferCount; i++)
	{
		auto bacBufferMemory = D12HeapMemory(memory.pointer + i, 1);

		ID3D12Resource* bacBufferResource;
		ASSERT_SUCCEEDED(d12SwapChain->GetBuffer(i, IID_PPV_ARGS(&bacBufferResource)));
		device->CreateRenderTargetView(bacBufferResource, nullptr, rtvHeap->GetCpuHandle(bacBufferMemory));

		auto bacBuffer = new D12Texture(swapChain->width, swapChain->height);
		bacBuffer->usage = TextureUsageFlagRender;
		bacBuffer->rtvMemory = bacBufferMemory;
		bacBuffer->resource = bacBufferResource;
		bacBuffer->currentState = D3D12_RESOURCE_STATE_PRESENT;
		swapChain->bacBuffers[i] = bacBuffer;
		SetName(bacBufferResource, L"BacBuffer %d (SwapChain %d)", i, resourceCounter);
	}
}

void D12GraphicsModule::InitializeTexture(D12Texture* texture)
{
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Width = texture->width;
	textureDesc.Height = texture->height;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	if (texture->usage & TextureUsageFlagRender)
		textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE clearValue;
	bool useClearVlaue = false;

	// If we match the clear values with render pass, we win performance
	if (texture->usage & TextureUsageFlagRender)
	{
		clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		clearValue.Color[0] = 0;
		clearValue.Color[1] = 0.2f;
		clearValue.Color[2] = 0.4f;
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
	if (texture->usage & TextureUsageFlagShader)
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
	if (texture->usage & TextureUsageFlagRender)
	{
		texture->rtvMemory = rtvHeap->Allocate(1);
		device->CreateRenderTargetView(texture->resource, nullptr, rtvHeap->GetCpuHandle(texture->rtvMemory));
	}
}

inline void D12GraphicsModule::InitializeFilter(D12Filter* filter)
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

void D12GraphicsModule::InitializeRenderPass(D12RenderPass* renderPass)
{
}

void D12GraphicsModule::InitializeBuffer(D12Buffer* target)
{
	target->currentState = D3D12_RESOURCE_STATE_GENERIC_READ;
	/*ASSERT_SUCCEEDED(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(target->data.size),
		target->currentState,
		nullptr,
		IID_PPV_ARGS(&target->resource)));
	target->cachedResourceGpuVirtualAddress = target->resource->GetGPUVirtualAddress();*/
	auto size = Math::GetPadded(target->GetSize(), (size_t)256);
	target->memory = bufferUploadHeap->Allocate(size);
	target->cachedResourceGpuVirtualAddress = bufferUploadHeap->GetOffset(target->memory);
	target->resource = bufferUploadHeap->Get_heap();
}

void D12GraphicsModule::CompilePipeline(D12ShaderPipeline* pipeline)
{
	// TODO: Make it universal
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ComPtr<ID3DBlob> error;
	D3DCompile(pipeline->source, pipeline->sourceSize, nullptr, nullptr, nullptr, "VertMain", "vs_5_0", compileFlags, 0, &vertexShader, &error);
	if (error.Get() != NULL)
		ERROR((const char*) error.Get()->GetBufferPointer());

	D3DCompile(pipeline->source, pipeline->sourceSize, nullptr, nullptr, nullptr, "FragMain", "ps_5_0", compileFlags, 0, &pixelShader, &error);
	if (error.Get() != NULL)
		ERROR((const char*) error.Get()->GetBufferPointer());

	pipeline->programs[ShaderProgramTypeVertex] = ShaderProgram(new uint8_t[vertexShader.Get()->GetBufferSize()], vertexShader.Get()->GetBufferSize());
	pipeline->programs[ShaderProgramTypeFragment] = ShaderProgram(new uint8_t[pixelShader.Get()->GetBufferSize()], pixelShader.Get()->GetBufferSize());

	memcpy((void*) pipeline->programs[ShaderProgramTypeVertex].code, (const uint8_t*) vertexShader.Get()->GetBufferPointer(), vertexShader.Get()->GetBufferSize());
	memcpy((void*) pipeline->programs[ShaderProgramTypeFragment].code, (const uint8_t*) pixelShader.Get()->GetBufferPointer(), pixelShader.Get()->GetBufferSize());
}

void D12GraphicsModule::InitializePipeline(D12ShaderPipeline* pipeline)
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
		if (pipeline->programs[ShaderProgramTypeVertex].available)
		{
			auto shaderProgram = pipeline->programs[ShaderProgramTypeVertex];
			ASSERT_SUCCEEDED(D3DCreateBlob(shaderProgram.size, &vertexShader));
			memcpy(vertexShader.Get()->GetBufferPointer(), shaderProgram.code, shaderProgram.size);
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		}

		if (pipeline->programs[ShaderProgramTypeFragment].available)
		{
			auto shaderProgram = pipeline->programs[ShaderProgramTypeFragment];
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
			case ShaderParameterTypeConstantBuffer:
			{
				rootParameters.push_back(D12RootParamter::AsConstantBuffer(parameter.name.c_str()));
				break;
			}
			case ShaderParameterTypeBuffer:
			{
				auto& rootParamter = pipeline->FindRootParameter(D12RootParamterTypeTableSRV);
				rootParamter.supParameters.push_back(D12RootSubParamter(parameter.name.c_str(), D3D12_DESCRIPTOR_RANGE_TYPE_SRV));
				break;
			}
			case ShaderParameterTypeTexture:
			{
				auto& rootParamter = pipeline->FindRootParameter(D12RootParamterTypeTableSRV);
				rootParamter.supParameters.push_back(D12RootSubParamter(parameter.name.c_str(), D3D12_DESCRIPTOR_RANGE_TYPE_SRV, true));
				break;
			}
			case ShaderParameterTypeSampler:
			{
				auto& rootParamter = pipeline->FindRootParameter(D12RootParamterTypeTableSamplers);
				rootParamter.supParameters.push_back(D12RootSubParamter(parameter.name.c_str(), D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER));
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
			case D12RootParamterTypeConstantBuffer:
			{
				d12RootParameters[i].InitAsConstantBufferView(i, 0U, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_ALL);
				break;
			}
			case D12RootParamterTypeTableSRV:
			{
				uint32_t srvCount = 0;
				rootParameter.GetCounts(&srvCount, nullptr);

				CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
				ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, srvCount, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

				d12RootParameters[i].InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_ALL);
				break;
			}
			case D12RootParamterTypeTableSamplers:
			{
				uint32_t samplersCount = 0;
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
			ERROR((const char*)error.Get()->GetBufferPointer());

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

void D12GraphicsModule::InitializeProperties(D12ShaderArguments* target)
{
	auto pipeline = (D12ShaderPipeline*) target->pipeline;
	auto& rootParameters = pipeline->rootParameters;
	auto& rootArguments = target->rootArguments;
	for (auto& rootParameter : rootParameters)
	{
		switch (rootParameter.type)
		{
		case D12RootParamterTypeTableSRV:
		{
			auto memory = srvHeap->Allocate(rootParameter.supParameters.size());
			rootArguments.push_back(D12RootArgument(memory));
			break;
		}
		case D12RootParamterTypeTableSamplers:
		{
			auto memory = samplersHeap->Allocate(rootParameter.supParameters.size());
			rootArguments.push_back(D12RootArgument(memory));
			break;
		}
		case D12RootParamterTypeConstantBuffer:
		{
			rootArguments.push_back(D12RootArgument(0));
			break;
		}
		}
	}
}

void D12GraphicsModule::InitializeBlitCopy(D12BlitCopy* target)
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
	vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeTypePosition, ColorFormatRGB24));
	vertexLayout.attributes.push_back(VertexAttributeLayout(VertexAttributeTypeTexCoord0, ColorFormatRG16));

	auto shaderDesc = new ShaderPipelineDesc();
	shaderDesc->name = "Test";
	shaderDesc->source = (const uint8_t*) source;
	shaderDesc->sourceSize = strlen(source);
	shaderDesc->states.zTest = ZTestLEqual;
	shaderDesc->states.zWrite = ZWriteOn;
	shaderDesc->varation = 0;
	shaderDesc->vertexLayout = vertexLayout;
	shaderDesc->parameters.push_back(ShaderParameter("_mainTex", ShaderParameterTypeTexture));
	shaderDesc->parameters.push_back(ShaderParameter("_mainTexSampler", ShaderParameterTypeSampler));
	target->pipeline = new D12ShaderPipeline(shaderDesc);
	InitializePipeline((D12ShaderPipeline*)target->pipeline);

	target->properties = new D12ShaderArguments(target->pipeline);
	InitializeProperties((D12ShaderArguments*) target->properties);

	static float vertices[] = {
		-1, -1, 0,	0, 0,
		-1, 1, 0,	0, 1,
		1, 1, 0,	1, 1,

		-1, -1, 0,	0, 0,
		1, 1, 0,	1, 1,
		1, -1, 0,	1, 0,
	};
	target->vertexBuffer = new D12Buffer(sizeof(vertices));
	InitializeBuffer((D12Buffer*) target->vertexBuffer);
	UpdateBuffer((D12Buffer*) target->vertexBuffer, 0, Range<uint8_t>((const uint8_t*) vertices, sizeof(vertices)));

	target->offset = 0;
	target->size = 3*2;

	target->renderPass = new D12RenderPass();
	InitializeRenderPass(target->renderPass);

	target->filter = new D12Filter(FilterOptions());
	InitializeFilter(target->filter);
	SetFilter((D12ShaderArguments*) target->properties, "_mainTexSampler", target->filter);
}

DXGI_FORMAT D12GraphicsModule::Convert(ColorFormat format)
{
	// TODO: Table lookup
	switch (format)
	{
	case ColorFormatRGBA32:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ColorFormatRGB24:
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case ColorFormatRG16:
		return DXGI_FORMAT_R32G32_FLOAT;
	}
	
	ERROR("Format is not convertable");
	return (DXGI_FORMAT) -1;
}

const char* D12GraphicsModule::Convert(VertexAttributeType type)
{
	// TODO: Table lookup
	switch (type)
	{
	case VertexAttributeTypePosition:
		return "POSITION";
	case VertexAttributeTypeTexCoord0:
		return "TEXCOORD";
	}

	ERROR("Type is not convertable");
	return nullptr;
}

uint32_t D12GraphicsModule::GetSize(ColorFormat format)
{
	switch (format)
	{
	case ColorFormatRGBA32:
		return sizeof(float) * 4;
	case ColorFormatRGB24:
		return sizeof(float) * 3;
	case ColorFormatRG16:
		return sizeof(float) * 2;
	}

	ERROR("Format is not convertable");
	return -1;
}