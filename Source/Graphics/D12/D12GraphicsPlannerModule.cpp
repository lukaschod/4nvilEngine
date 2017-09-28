#include <Graphics\D12\D12GraphicsPlannerModule.h>
#include <Graphics\D12\D12GraphicsModule.h>
#include <Graphics\D12\D12GraphicsExecuterModule.h>
#include <Graphics\D12\D12Heap.h>

D12GraphicsPlannerModule::D12GraphicsPlannerModule(ID3D12Device* device) :
	device(device)
{
	directQueue = new D12CmdQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	directAllocatorPool = new D12CmdAllocatorPool(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

void D12GraphicsPlannerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	ExecuteAfter<CmdBufferPoolModule>(moduleManager);
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

size_t D12GraphicsPlannerModule::GetExecutionkSize() { return recordedCmdBuffers.size(); }
size_t D12GraphicsPlannerModule::GetSplitExecutionSize(size_t currentSize) 
{ 
	if (currentSize == GetExecutionkSize())
		return recordedCmdBuffers.size() % 4 + recordedCmdBuffers.size() / 4;
	else
		return recordedCmdBuffers.size() / 4;
}

void D12GraphicsPlannerModule::Execute(const ExecutionContext& context)
{
	auto allocatorPool = directAllocatorPool->TryPull(directQueue->GetCompletedBufferIndex());
	uint64_t lastBufferIndex = 0;
	for (uint32_t j = context.offset; j < context.offset + context.size; j++)
	{
		auto buffer = recordedCmdBuffers[j];
		auto& stream = buffer->stream;
		directQueue->Reset(buffer, allocatorPool);
		for (int i = 0; i < buffer->commandCount; i++)
		{
			uint32_t commandCode;
			stream.Read(commandCode);
			ASSERT(ExecuteCommand(context, buffer, commandCode));
		}
		directQueue->Close(buffer);
		executer->RecordCmdBuffer(context, buffer);
		lastBufferIndex = buffer->index;
	}
	directAllocatorPool->Push(lastBufferIndex, allocatorPool);
}

DECLARE_COMMAND_CODE(PushDebug);
void D12GraphicsPlannerModule::RecordPushDebug(const char* name)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(kCommandCodePushDebug);
	stream.Write(name);
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(PopDebug);
void D12GraphicsPlannerModule::RecordPopDebug()
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(kCommandCodePopDebug);
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetTextureState);
void D12GraphicsPlannerModule::RecordSetTextureState(const D12Texture* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(kCommandCodeSetTextureState);
	stream.Write(target);
	stream.Write(currentState);
	stream.Write(nextState);
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetBufferState);
void D12GraphicsPlannerModule::RecordSetBufferState(const D12Buffer* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(kCommandCodeSetBufferState);
	stream.Write(target);
	stream.Write(currentState);
	stream.Write(nextState);
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetRenderPass);
void D12GraphicsPlannerModule::RecordSetRenderPass(const D12RenderPass* target)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(kCommandCodeSetRenderPass);
	stream.Write(*target); // We need copy here, because renderpass lives on cpu
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(UpdateBuffer);
void D12GraphicsPlannerModule::RecordUpdateBuffer(const D12Buffer* target, uint32_t targetOffset, Range<uint8_t> data)
{ 
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(kCommandCodeUpdateBuffer);
	stream.Write(target);
	stream.Write(targetOffset);
	stream.Write(data);
	buffer->commandCount++;
}

void D12GraphicsPlannerModule::RecordPresent(const D12SwapChain* swapchain)
{
	SplitRecording();
	auto buffer = ContinueRecording();
	ASSERT(buffer->swapChain == nullptr);
	buffer->swapChain = swapchain->GetIDXGISwapChain3();
}

DECLARE_COMMAND_CODE(DrawSimple);
void D12GraphicsPlannerModule::RecordDrawSimple(const DrawSimple& target)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(kCommandCodeDrawSimple);
	stream.Write(target);

	auto& rootParameters = ((D12ShaderPipeline*) target.pipeline)->rootParameters;
	auto& rootArguments = ((D12ShaderArguments*) target.properties)->rootArguments;
	for (size_t i = 0; i < rootParameters.size(); i++)
	{
		auto& rootParameter = rootParameters[i];
		auto& rootArgument = rootArguments[i];

		switch (rootParameter.type)
		{
		case kD12RootParamterTypeTableSRV:
		case kD12RootParamterTypeTableSamplers:
		{
			stream.Write(rootArgument.memory);
			break;
		}
		case kD12RootParamterTypeConstantBuffer:
		{
			stream.Write(rootArgument.subData);
			break;
		}
		}
	}

	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(SetHeap);
void D12GraphicsPlannerModule::RecordSetHeap(const D12Heap* heap)
{
	auto buffer = ContinueRecording();
	auto& stream = buffer->stream;
	stream.Write(kCommandCodeSetHeap);
	stream.Write(heap);
	buffer->commandCount++;
}

void D12GraphicsPlannerModule::RecordRequestSplit()
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

		DESERIALIZE_METHOD_ARG1_START(SetRenderPass, D12RenderPass, target);
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
			buffer->heaps[kD12HeapTypeSRVs]->Get_heap(),
			buffer->heaps[kD12HeapTypeSamplers]->Get_heap() };
		commandList->SetDescriptorHeaps(2, heaps);

		for (int i = 0; i < count; i++)
		{
			auto& color = target.colors[i];
			if (color.loadAction == kLoadActionClear)
			{
				const float clearColor[] = { color.clearColor.r, color.clearColor.g, color.clearColor.b, color.clearColor.a };
				commandList->ClearRenderTargetView(target.colorDescriptors[i], clearColor, 0, nullptr);
			}
		}

		auto& depth = target.depth;
		if (depth.texture != nullptr && depth.loadAction == kLoadActionClear)
		{
			commandList->ClearDepthStencilView(target.depthDescriptor, D3D12_CLEAR_FLAG_DEPTH, target.depth.clearDepth, 0, 0, nullptr);
		}
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(UpdateBuffer, const D12Buffer*, target, uint32_t, targetOffset, Range<uint8_t>, data);
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ASSERT_SUCCEEDED(target->resource->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin + targetOffset, data.pointer, data.size);
		target->resource->Unmap(0, nullptr);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(DrawSimple, DrawSimple, target);
		auto pipeline = (const D12ShaderPipeline*) target.pipeline;
		commandList->SetPipelineState(pipeline->pipelineState);
		commandList->SetGraphicsRootSignature(pipeline->rootSignature);
		auto& rootParameters = ((D12ShaderPipeline*) target.pipeline)->rootParameters;
		auto& rootArguments = ((D12ShaderArguments*) target.properties)->rootArguments;
		for (size_t i = 0; i < rootParameters.size(); i++)
		{
			auto& rootParameter = rootParameters[i];
			auto& rootArgument = rootArguments[i];
			
			switch (rootParameter.type)
			{
			case kD12RootParamterTypeTableSRV:
			{
				ASSERT(buffer->heaps[kD12HeapTypeSRVs] != nullptr);
				D12HeapMemory value; stream.Read<D12HeapMemory>(value);
				commandList->SetGraphicsRootDescriptorTable((UINT) i, buffer->heaps[kD12HeapTypeSRVs]->GetGpuHandle(value));
				break;
			}
			case kD12RootParamterTypeTableSamplers:
			{
				ASSERT(buffer->heaps[kD12HeapTypeSamplers] != nullptr);
				D12HeapMemory value; stream.Read<D12HeapMemory>(value);
				commandList->SetGraphicsRootDescriptorTable((UINT) i, buffer->heaps[kD12HeapTypeSamplers]->GetGpuHandle(value));
				break;
			}
			case kD12RootParamterTypeConstantBuffer:
			{
				D3D12_GPU_VIRTUAL_ADDRESS value; stream.Read<D3D12_GPU_VIRTUAL_ADDRESS>(value);
				commandList->SetGraphicsRootConstantBufferView((UINT) i, value);
				break;
			}
			}
		}
		auto vertexBufferView = pipeline->vertexBuffer;
		auto vertexBuffer = (const D12Buffer*) target.vertexBuffer;
		vertexBufferView.BufferLocation = vertexBuffer->resource->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = (UINT) vertexBuffer->data.size;
		ASSERT(vertexBufferView.SizeInBytes % vertexBufferView.StrideInBytes == 0);
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->DrawInstanced(target.size, 1, target.offset, 0);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(SetHeap, D12Heap*, heap);
		buffer->heaps[heap->Get_type()] = heap;
		DESERIALIZE_METHOD_END;
	}
	return false;
}