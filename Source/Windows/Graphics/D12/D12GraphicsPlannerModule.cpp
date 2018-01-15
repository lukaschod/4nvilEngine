#include <Windows\Graphics\D12\D12GraphicsPlannerModule.h>
#include <Windows\Graphics\D12\D12GraphicsModule.h>
#include <Windows\Graphics\D12\D12GraphicsExecuterModule.h>
#include <Windows\Graphics\D12\D12DescriptorHeap.h>

D12GraphicsPlannerModule::D12GraphicsPlannerModule(ID3D12Device* device) 
	: device(device)
	, drawOptimizers(4)
{
	directQueue = new D12CmdQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	directAllocatorPool = new D12CmdAllocatorPool(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

void D12GraphicsPlannerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	Module::SetupExecuteOrder(moduleManager);
	executer = ExecuteBefore<D12GraphicsExecuterModule>(moduleManager);
}

D12CmdBuffer* D12GraphicsPlannerModule::ContinueRecording()
{
	if (recordedCmdBuffers.empty())
		SplitRecording();
	return recordedCmdBuffers.back();
}

void D12GraphicsPlannerModule::SplitRecording()
{
	auto buffer = directQueue->Pull();
	recordedCmdBuffers.push_back(buffer);
}

uint64_t D12GraphicsPlannerModule::GetRecordingBufferIndex()
{
	return ContinueRecording()->index;
}

uint64_t D12GraphicsPlannerModule::GetCompletedBufferIndex()
{
	return executer->Get_completedBufferIndex();
}

size_t D12GraphicsPlannerModule::GetExecutionSize() { return recordedCmdBuffers.size(); }
size_t D12GraphicsPlannerModule::GetSplitExecutionSize(size_t currentSize) 
{ 
	if (currentSize == GetExecutionSize())
		return recordedCmdBuffers.size() % 4 + recordedCmdBuffers.size() / 4;
	else
		return recordedCmdBuffers.size() / 4;
}

void D12GraphicsPlannerModule::Execute(const ExecutionContext& context)
{
	PROFILE_FUNCTION;

	// Pull CmdAllocator this is where all our commands will be stored physicaly
	auto allocatorPool = directAllocatorPool->TryPull(directQueue->GetCompletedBufferIndex());
	D12CmdBuffer* buffer = nullptr;
	D12CmdBuffer* mainBuffer = recordedCmdBuffers[context.offset];

	// Pull CmdList this interface that will allow use to store commands in allocator
	directQueue->Reset(mainBuffer, allocatorPool);

	auto totalCommandCount = 0;
	
	for (uint32_t j = context.offset; j < context.offset + context.size; j++)
	{
		buffer = recordedCmdBuffers[j];
		auto& stream = buffer->stream;
		stream.Reset();

		// TODO: Technical dept, need some clean solution for CmdList accessing
		auto cachedCmdList = buffer->commandList;
		buffer->commandList = mainBuffer->commandList;

		for (int i = 0; i < buffer->commandCount; i++)
		{
			auto& commandCode = stream.FastRead<uint32_t>();
			ASSERT(ExecuteCommand(context, buffer, commandCode));
		}

		// TODO: Technical dept, need some clean solution for CmdList accessing
		buffer->commandList = cachedCmdList;

		totalCommandCount += buffer->commandCount;

		if (buffer->swapChain)
			executer->RecCmdBuffer(context, buffer);
	}

	directQueue->Close(mainBuffer);

	executer->RecCmdBuffer(context, mainBuffer);

	// Push back allocator and mark it as free once the buffer finished
	directAllocatorPool->Push(buffer->index, allocatorPool);

	// Reset optimizer for new frame
	auto& drawOptimizer = drawOptimizers[context.workerIndex];
	drawOptimizer.Clear();
}

DECLARE_COMMAND_CODE(PushDebug);
void D12GraphicsPlannerModule::RecPushDebug(const char* name)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(CommandCodePushDebug);
	stream.Write(name);
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(PopDebug);
void D12GraphicsPlannerModule::RecPopDebug()
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(CommandCodePopDebug);
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetTextureState);
void D12GraphicsPlannerModule::RecSetTextureState(const D12Texture* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(CommandCodeSetTextureState);
	stream.Write(target);
	stream.Write(currentState);
	stream.Write(nextState);
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetBufferState);
void D12GraphicsPlannerModule::RecSetBufferState(const D12Buffer* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(CommandCodeSetBufferState);
	stream.Write(target);
	stream.Write(currentState);
	stream.Write(nextState);
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetRenderPass);
void D12GraphicsPlannerModule::RecSetRenderPass(const D12RenderPass* target, bool ignoreLoadActions)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(CommandCodeSetRenderPass);
	recordingOptimizer.MarSetRenderPass((D12RenderPass*)target);
	stream.Write((void*)target, sizeof(D12RenderPass)); // We need copy here, because renderpass lives on cpu
	stream.Write(ignoreLoadActions);
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(UpdateBuffer);
void D12GraphicsPlannerModule::RecUpdateBuffer(const D12Buffer* target, uint32_t targetOffset, Range<uint8_t> data)
{ 
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(CommandCodeUpdateBuffer);
	stream.Write(target);
	stream.Write(targetOffset);
	stream.Write(data);
	buffer->commandCount++;
}

void D12GraphicsPlannerModule::RecPresent(const D12SwapChain* swapchain)
{
	SplitRecording();
	auto buffer = ContinueRecording();
	ASSERT(buffer->swapChain == nullptr);
	buffer->swapChain = swapchain->GetIDXGISwapChain3();
}

DECLARE_COMMAND_CODE(Draw);
void D12GraphicsPlannerModule::RecDraw(const DrawDesc& target)
{
	// Lets try to split big command lists this way we can distribut work accross workers
	if (recordingOptimizer.ShouldSplitRecording())
	{
		SplitRecording();
		RecSetHeap((const D12DescriptorHeap**) recordingOptimizer.lastHeaps);
		RecSetRenderPass(recordingOptimizer.lastRenderPass, true);
	}

	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(CommandCodeDraw);
	stream.Write(target);

	auto& rootParameters = ((D12ShaderPipeline*) target.pipeline)->rootParameters;
	auto& rootArguments = ((D12ShaderArguments*) target.properties)->rootArguments;
	for (size_t i = 0; i < rootParameters.size(); i++)
	{
		auto& rootParameter = rootParameters[i];
		auto& rootArgument = rootArguments[i];

		switch (rootParameter.type)
		{
		case D12RootParamterTypeTableSRV:
		case D12RootParamterTypeTableSamplers:
		{
			stream.Write(rootArgument.memory);
			break;
		}
		case D12RootParamterTypeConstantBuffer:
		{
			stream.Write(rootArgument.subData);
			break;
		}
		}
	}

	buffer->commandCount++;
	recordingOptimizer.MarDraw();
}

DECLARE_COMMAND_CODE(SetHeap);
void D12GraphicsPlannerModule::RecSetHeap(const D12DescriptorHeap** heap)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(CommandCodeSetHeap);
	recordingOptimizer.MarSetHeap((D12DescriptorHeap**) heap);
	stream.Write(heap, sizeof(D12DescriptorHeap*) * D12HeapTypeCount);
	buffer->commandCount++;
}

void D12GraphicsPlannerModule::RecRequestSplit()
{
	SplitRecording();
}

void D12GraphicsPlannerModule::Reset()
{
	for (auto& cmdBuffer : recordedCmdBuffers)
		directQueue->Push(cmdBuffer);
	recordedCmdBuffers.clear(); // Make sure no allocation happens
}

ID3D12CommandQueue* D12GraphicsPlannerModule::GetDirectQueue()
{
	return directQueue->Get_queue();
}

bool D12GraphicsPlannerModule::ExecuteCommand(const ExecutionContext& context, D12CmdBuffer* buffer, uint32_t commandCode)
{
	auto& stream = buffer->stream;
	auto commandList = (ID3D12GraphicsCommandList*)buffer->commandList;
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(PushDebug, const char*, name);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_START(PopDebug);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetTextureState, const D12Texture*, target, D3D12_RESOURCE_STATES, currentState, D3D12_RESOURCE_STATES, nextState);
		EXT_TRACE("target=%d currentState=%d nextState=%d", target, currentState, nextState);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(target->resource, currentState, nextState));
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetBufferState, const D12Buffer*, target, D3D12_RESOURCE_STATES, currentState, D3D12_RESOURCE_STATES, nextState);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(target->resource, currentState, nextState));
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetRenderPass, D12RenderPass, target, bool, ignoreLoadActions);
		auto count = target.colorDescriptorsCount;
		ASSERT(count != 0);
		if (target.depth.texture != nullptr)
			commandList->OMSetRenderTargets((UINT) count, target.colorDescriptors, false, &target.depthDescriptor);
		else
			commandList->OMSetRenderTargets((UINT) count, target.colorDescriptors, false, nullptr);
		commandList->RSSetViewports(1, &target.d12Viewport);
		commandList->RSSetScissorRects(1, &target.d12ScissorRect);

		// TODO: make it dynamic maybe, need to find out if staticlly assigning heaps is costly and having it assigned
		ID3D12DescriptorHeap* heaps[] = {
			buffer->heaps[D12HeapTypeSRVs]->Get_heap(),
			buffer->heaps[D12HeapTypeSamplers]->Get_heap() };
		commandList->SetDescriptorHeaps(2, heaps);

		if (ignoreLoadActions)
			return true;

		for (int i = 0; i < count; i++)
		{
			auto& color = target.colors[i];
			if (color.loadAction == LoadActionClear)
			{
				const float clearColor[] = { color.clearColor.r, color.clearColor.g, color.clearColor.b, color.clearColor.a };
				commandList->ClearRenderTargetView(target.colorDescriptors[i], clearColor, 0, nullptr);
			}
		}

		auto& depth = target.depth;
		if (depth.texture != nullptr && depth.loadAction == LoadActionClear)
		{
			commandList->ClearDepthStencilView(target.depthDescriptor, D3D12_CLEAR_FLAG_DEPTH, target.depth.clearDepth, 0, 0, nullptr);
		}
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(UpdateBuffer, const D12Buffer*, target, uint32_t, targetOffset, Range<uint8_t>, data);
		memcpy(target->resourceMappedPointer + targetOffset + target->resourceOffset, data.pointer, data.size);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(Draw, DrawDesc, target);
		auto& drawOptimizer = drawOptimizers[context.workerIndex];

		// If we used same pipeline lets skip
		auto pipeline = (const D12ShaderPipeline*) target.pipeline;
		if (drawOptimizer.lastPipeline != pipeline)
		{
			commandList->SetPipelineState(pipeline->pipelineState);
			commandList->SetGraphicsRootSignature(pipeline->rootSignature);
			drawOptimizer.lastPipeline = pipeline;
		}
		
		auto& rootParameters = ((D12ShaderPipeline*) target.pipeline)->rootParameters;
		auto& rootArguments = ((D12ShaderArguments*) target.properties)->rootArguments;
		for (size_t i = 0; i < rootParameters.size(); i++)
		{
			auto& rootParameter = rootParameters[i];
			auto& rootArgument = rootArguments[i];
			
			switch (rootParameter.type)
			{
			case D12RootParamterTypeTableSRV:
			{
				ASSERT(buffer->heaps[D12HeapTypeSRVs] != nullptr);
				auto& value = stream.FastRead<D12HeapMemory>();
				auto handle = buffer->heaps[D12HeapTypeSRVs]->GetGpuHandle(value);
				
				if (handle.ptr != drawOptimizer.rootArguments[i])
				{
					commandList->SetGraphicsRootDescriptorTable((UINT) i, handle);
					drawOptimizer.rootArguments[i] = handle.ptr;
				}
				
				break;
			}
			case D12RootParamterTypeTableSamplers:
			{
				ASSERT(buffer->heaps[D12HeapTypeSamplers] != nullptr);
				auto& value = stream.FastRead<D12HeapMemory>();
				auto handle = buffer->heaps[D12HeapTypeSamplers]->GetGpuHandle(value);

				if (handle.ptr != drawOptimizer.rootArguments[i])
				{
					commandList->SetGraphicsRootDescriptorTable((UINT) i, handle);
					drawOptimizer.rootArguments[i] = handle.ptr;
				}
				
				break;
			}
			case D12RootParamterTypeConstantBuffer:
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
			auto vertexBuffer = (const D12Buffer*) target.vertexBuffer;
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
		stream.Read(buffer->heaps, sizeof(D12DescriptorHeap*) * D12HeapTypeCount);
		DESERIALIZE_METHOD_END;
	}
	return false;
}