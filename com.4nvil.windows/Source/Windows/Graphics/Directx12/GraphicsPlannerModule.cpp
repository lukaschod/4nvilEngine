#include <Core\Tools\Enum.h>
#include <Windows\Graphics\Directx12\GraphicsPlannerModule.h>
#include <Windows\Graphics\Directx12\GraphicsModule.h>
#include <Windows\Graphics\Directx12\GraphicsExecutorModule.h>
#include <Windows\Graphics\Directx12\DescriptorHeap.h>

using namespace Core;
using namespace Windows;
using namespace Windows::Directx12;

GraphicsPlannerModule::GraphicsPlannerModule(ID3D12Device* device) 
	: device(device)
	, drawOptimizers(4)
{
	directQueue = new CmdQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	directAllocatorPool = new CmdAllocatorPool(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

void GraphicsPlannerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	executor = ExecuteBefore<GraphicsExecutorModule>(moduleManager);
}

Directx12::CmdBuffer* GraphicsPlannerModule::ContinueRecording()
{
	if (recordedCmdBuffers.empty())
		SplitRecording();
	return recordedCmdBuffers.back();
}

void GraphicsPlannerModule::SplitRecording()
{
	auto buffer = directQueue->Pull();
	recordedCmdBuffers.push_back(buffer);
}

uint64 GraphicsPlannerModule::GetRecordingBufferIndex()
{
	return ContinueRecording()->index;
}

uint64 GraphicsPlannerModule::GetCompletedBufferIndex()
{
	return executor->Get_completedBufferIndex();
}

size_t GraphicsPlannerModule::GetExecutionSize() { return recordedCmdBuffers.size(); }
size_t GraphicsPlannerModule::GetSplitExecutionSize() { return Math::SplitJobs(GetExecutionSize(), 4, 10); }

void GraphicsPlannerModule::Execute(const ExecutionContext& context)
{
	MARK_FUNCTION;

	// Pull CmdAllocator this is where all our commands will be stored physicaly
	auto allocatorPool = directAllocatorPool->TryPull(directQueue->GetCompletedBufferIndex());
	auto mainBuffer = recordedCmdBuffers[context.start];
	auto buffer = mainBuffer;

	// Pull CmdList this interface that will allow use to store commands in allocator
	directQueue->Reset(mainBuffer, allocatorPool);

	for (auto j = context.start; j < context.end; j++)
	{
		buffer = recordedCmdBuffers[j];
		auto& stream = buffer->stream;
		stream.Reset();

		// TODO: Technical dept, need some clean solution for CmdList accessing
		auto cachedCmdList = buffer->commandList;
		buffer->commandList = mainBuffer->commandList;

		for (int i = 0; i < buffer->commandCount; i++)
		{
			auto& commandCode = stream.FastRead<CommandCode>();
			ASSERT(ExecuteCommand(context, buffer, commandCode));
			stream.Align();
		}

		// TODO: Technical dept, need some clean solution for CmdList accessing
		buffer->commandList = cachedCmdList;

		if (buffer->swapChain)
			executor->RecCmdBuffer(context, buffer);
	}

	directQueue->Close(mainBuffer);

	if (mainBuffer->swapChain == nullptr)
		executor->RecCmdBuffer(context, mainBuffer);

	// Push back allocator and mark it as free once the buffer finished
	directAllocatorPool->Push(buffer->index, allocatorPool);

	// Reset optimizer for new frame
	auto& drawOptimizer = drawOptimizers[context.workerIndex];
	drawOptimizer.Clear();
}

DECLARE_COMMAND_CODE(PushDebug);
void GraphicsPlannerModule::RecPushDebug(const char* name)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(PushDebug));
	stream.Write(name);
	stream.Align();
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(PopDebug);
void GraphicsPlannerModule::RecPopDebug()
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(PopDebug));
	stream.Align();
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetTextureState);
void GraphicsPlannerModule::RecSetTextureState(const Texture* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(SetTextureState));
	stream.Write(target);
	stream.Write(currentState);
	stream.Write(nextState);
	stream.Align();
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetBufferState);
void GraphicsPlannerModule::RecSetBufferState(const Buffer* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(SetBufferState));
	stream.Write(target);
	stream.Write(currentState);
	stream.Write(nextState);
	stream.Align();
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetRenderPass);
void GraphicsPlannerModule::RecSetRenderPass(const RenderPass* target, bool ignoreLoadAction)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(SetRenderPass));
	recordingOptimizer.MarSetRenderPass((RenderPass*)target);
	stream.Write((void*)target, sizeof(RenderPass)); // We need copy here, because renderpass lives on cpu
	stream.Write(ignoreLoadAction);
	stream.Align();
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(UpdateBuffer);
void GraphicsPlannerModule::RecUpdateBuffer(const Buffer* target, uint32 targetOffset, Range<uint8> data)
{ 
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(UpdateBuffer));
	stream.Write(target);
	stream.Write(targetOffset);
	stream.Write(data);
	stream.Align();
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(CopyBuffer);
void GraphicsPlannerModule::RecCopyBuffer(const Buffer* source, const Buffer* destination, size_t size)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(CopyBuffer));
	stream.Write(source);
	stream.Write(destination);
	stream.Write(size);
	stream.Align();
	buffer->commandCount++;
}

void GraphicsPlannerModule::RecPresent(const SwapChain* swapchain)
{
	SplitRecording();
	auto buffer = ContinueRecording();
	ASSERT(buffer->swapChain == nullptr);
	buffer->swapChain = swapchain->GetIDXGISwapChain3();
}

DECLARE_COMMAND_CODE(Draw);
void GraphicsPlannerModule::RecDraw(const DrawDesc& target)
{
	// Lets try to split big command lists this way we can distribut work accross workers
	if (recordingOptimizer.ShouldSplitRecording())
	{
		SplitRecording();
		RecSetHeap((const DescriptorHeap**) recordingOptimizer.lastHeaps);
		RecSetRenderPass(recordingOptimizer.lastRenderPass, true);
	}

	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(Draw));
	stream.Write(target);

	auto& rootParameters = ((ShaderPipeline*) target.pipeline)->rootParameters;
	auto& rootArguments = ((ShaderArguments*) target.properties)->rootArguments;
	for (size_t i = 0; i < rootParameters.size(); i++)
	{
		auto& rootParameter = rootParameters[i];
		auto& rootArgument = rootArguments[i];

		switch (rootParameter.type)
		{
		case RootParamterType::TableSRV:
		case RootParamterType::TableSamplers:
		{
			stream.Write(rootArgument.memory);
			break;
		}
		case RootParamterType::ConstantBuffer:
		{
			stream.Write(rootArgument.subData);
			break;
		}
		}
	}

	stream.Align();
	buffer->commandCount++;
	recordingOptimizer.MarDraw();
}

DECLARE_COMMAND_CODE(SetHeap);
void GraphicsPlannerModule::RecSetHeap(const DescriptorHeap** heap)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(SetHeap));
	recordingOptimizer.MarSetHeap((DescriptorHeap**) heap);
	stream.Write(heap, sizeof(DescriptorHeap*) * Enum::ToUnderlying(HeapType::Count));
	stream.Align();
	buffer->commandCount++;
}

void GraphicsPlannerModule::RecRequestSplit()
{
	SplitRecording();
}

void GraphicsPlannerModule::Reset()
{
	for (auto& cmdBuffer : recordedCmdBuffers)
		directQueue->Push(cmdBuffer);
	recordedCmdBuffers.clear(); // Make sure no allocation happens
}

ID3D12CommandQueue* GraphicsPlannerModule::GetDirectQueue()
{
	return directQueue->Get_queue();
}

bool GraphicsPlannerModule::ExecuteCommand(const ExecutionContext& context, Directx12::CmdBuffer* buffer, CommandCode commandCode)
{
	auto& stream = buffer->stream;
	auto commandList = (ID3D12GraphicsCommandList*)buffer->commandList;
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(PushDebug, const char*, name);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_START(PopDebug);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetTextureState, const Texture*, target, D3D12_RESOURCE_STATES, currentState, D3D12_RESOURCE_STATES, nextState);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(target->resource, currentState, nextState));
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetBufferState, const Buffer*, target, D3D12_RESOURCE_STATES, currentState, D3D12_RESOURCE_STATES, nextState);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(target->resource, currentState, nextState));
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetRenderPass, RenderPass, target, bool, ignoreLoadAction);
		auto count = target.colorDescriptorsCount;
		ASSERT(count != 0);
		if (target.depth.texture != nullptr)
			commandList->OMSetRenderTargets((UINT) count, target.colorDescriptors, false, &target.depthDescriptor);
		else
			commandList->OMSetRenderTargets((UINT) count, target.colorDescriptors, false, nullptr);
		commandList->RSSetViewports(1, &target.d12Viewport);
		commandList->RSSetScissorRects(1, &target.d12ScissorRect);

		// TODO: make it dynamic maybe, need to find out if staticlly assigning heaps is costly and having it assigned
		// TODO: Technical depth GetDesciptorHeap
		ID3D12DescriptorHeap* heaps[] = {
			buffer->heaps[Enum::ToUnderlying(HeapType::SRVs)]->GetDesciptorHeap(),
			buffer->heaps[Enum::ToUnderlying(HeapType::Samplers)]->GetDesciptorHeap() };
		commandList->SetDescriptorHeaps(2, heaps);

		if (ignoreLoadAction)
			return true;

		for (int i = 0; i < count; i++)
		{
			auto& color = target.colors[i];
			if (color.loadAction == LoadAction::Clear)
			{
				const float clearColor[] = { color.clearColor.r, color.clearColor.g, color.clearColor.b, color.clearColor.a };
				commandList->ClearRenderTargetView(target.colorDescriptors[i], clearColor, 0, nullptr);
			}
		}

		auto& depth = target.depth;
		if (depth.texture != nullptr && depth.loadAction == LoadAction::Clear)
		{
			commandList->ClearDepthStencilView(target.depthDescriptor, D3D12_CLEAR_FLAG_DEPTH, target.depth.clearDepth, 0, 0, nullptr);
		}
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(UpdateBuffer, const Buffer*, target, uint32, targetOffset, Range<uint8>, data);
		ASSERT(target->resourceMappedPointer != nullptr && data.pointer != nullptr && data.size != 0);
		memcpy(target->resourceMappedPointer + targetOffset, data.pointer, data.size);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(CopyBuffer, const Buffer*, src, const Buffer*, dst, size_t, size);
		commandList->CopyBufferRegion(dst->resource, dst->resourceOffset, src->resource, src->resourceOffset, size);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(Draw, DrawDesc, target);
		auto& drawOptimizer = drawOptimizers[context.workerIndex];

		// If we used same pipeline lets skip
		auto pipeline = (const ShaderPipeline*) target.pipeline;
		if (drawOptimizer.lastPipeline != pipeline)
		{
			commandList->SetPipelineState(pipeline->pipelineState);
			commandList->SetGraphicsRootSignature(pipeline->rootSignature);
			drawOptimizer.lastPipeline = pipeline;
		}
		
		auto& rootParameters = ((ShaderPipeline*) target.pipeline)->rootParameters;
		auto& rootArguments = ((ShaderArguments*) target.properties)->rootArguments;
		for (size_t i = 0; i < rootParameters.size(); i++)
		{
			auto& rootParameter = rootParameters[i];
			auto& rootArgument = rootArguments[i];
			
			switch (rootParameter.type)
			{
			case RootParamterType::TableSRV:
			{
				ASSERT(buffer->heaps[Enum::ToUnderlying(HeapType::SRVs)] != nullptr);
				auto& value = stream.FastRead<HeapMemory>();
				auto handle = buffer->heaps[Enum::ToUnderlying(HeapType::SRVs)]->GetGpuHandle(value);
				
				if (handle.ptr != drawOptimizer.rootArguments[i])
				{
					commandList->SetGraphicsRootDescriptorTable((UINT) i, handle);
					drawOptimizer.rootArguments[i] = handle.ptr;
				}
				
				break;
			}
			case RootParamterType::TableSamplers:
			{
				ASSERT(buffer->heaps[Enum::ToUnderlying(HeapType::Samplers)] != nullptr);
				auto& value = stream.FastRead<HeapMemory>();
				auto handle = buffer->heaps[Enum::ToUnderlying(HeapType::Samplers)]->GetGpuHandle(value);

				if (handle.ptr != drawOptimizer.rootArguments[i])
				{
					commandList->SetGraphicsRootDescriptorTable((UINT) i, handle);
					drawOptimizer.rootArguments[i] = handle.ptr;
				}
				
				break;
			}
			case RootParamterType::ConstantBuffer:
			{
				auto& value = stream.FastRead<D3D12_GPU_VIRTUAL_ADDRESS>();

				if (value != drawOptimizer.rootArguments[i])
				{
					commandList->SetGraphicsRootConstantBufferView((UINT) i, value);
					drawOptimizer.rootArguments[i] = value;
				}
				
				break;
			}
			}
		}

		// If we used same pipeline and vertex buffer lets skip
		if (drawOptimizer.lastPipeline != pipeline || drawOptimizer.lastVertexBuffer != target.vertexBuffer)
		{
			auto vertexBufferView = pipeline->vertexBuffer;
			auto vertexBuffer = (const Buffer*) target.vertexBuffer;
			vertexBufferView.BufferLocation = vertexBuffer->cachedResourceGpuVirtualAddress;
			vertexBufferView.SizeInBytes = (UINT) vertexBuffer->data.size;
			ASSERT(vertexBufferView.SizeInBytes % vertexBufferView.StrideInBytes == 0);
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			drawOptimizer.lastVertexBuffer = target.vertexBuffer;
		}

		commandList->DrawInstanced(target.size, 1, target.offset, 0);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_START(SetHeap);
		stream.Read(buffer->heaps, sizeof(DescriptorHeap*) * Enum::ToUnderlying(HeapType::Count));
		DESERIALIZE_METHOD_END;
	}
	return false;
}