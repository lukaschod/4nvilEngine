#include "D12CmdQueue.h"

D12CmdQueue::D12CmdQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type) :
	device(device),
	type(type),
	pulledBufferCount(0)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = type;
	queueDesc.NodeMask = 1;
	ASSERT_SUCCEEDED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue)));
	
	ASSERT_SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

	waitBufferCompletionEvent = CreateEvent(nullptr, false, false, nullptr);
	ASSERT_SUCCEEDED(waitBufferCompletionEvent != INVALID_HANDLE_VALUE);
}

D12CmdQueue::~D12CmdQueue()
{
	for (auto buffer : buffers)
	{
		if (buffer->commandList != nullptr)
			buffer->commandList->Release();
	}
	queue->Release();
	fence->Release();
	CloseHandle(waitBufferCompletionEvent);
}

D12CmdBuffer* D12CmdQueue::Pull()
{
	if (readyBuffers.empty())
	{
		auto readyBuffer = new D12CmdBuffer(this, type);
		readyBuffer->Reset(pulledBufferCount++);
		return readyBuffer;
	}
	else
	{
		auto readyBuffer = readyBuffers.front();
		readyBuffer->Reset(pulledBufferCount++);
		readyBuffers.pop();
		return readyBuffer;
	}

	return nullptr;
}

void D12CmdQueue::Push(D12CmdBuffer * buffer)
{
	readyBuffers.push(buffer);
}

void D12CmdQueue::Reset(D12CmdBuffer* buffer, ID3D12CommandAllocator* allocator)
{
	if (buffer->commandList == nullptr)
	{
		ASSERT_SUCCEEDED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, nullptr, IID_PPV_ARGS(&buffer->commandList)));
	}
	else
	{
		ASSERT_SUCCEEDED(buffer->commandList->Reset(allocator, nullptr));
	}
	buffer->stream.Reset();
}

void D12CmdQueue::Close(D12CmdBuffer* buffer)
{
	buffer->commandList->Close();
}

void D12CmdQueue::Execute(D12CmdBuffer* buffer, bool isLast)
{
	if (buffer->swapChain != nullptr)
		buffer->swapChain->Present(1, 0);
	else
	{
		ASSERT_SUCCEEDED(buffer->commandList != nullptr);
		queue->ExecuteCommandLists(1, (ID3D12CommandList**) &buffer->commandList);
	}
	queue->Signal(fence, buffer->index);
}

uint64_t D12CmdQueue::GetCompletedBufferIndex()
{
	// TODO: Sync?
	return fence->GetCompletedValue();
}

void D12CmdQueue::WaitForBufferIndexToComplete(uint64_t index)
{
	if (index <= fence->GetCompletedValue())
		return;

	{
		std::lock_guard<std::mutex> lock(waitBufferCompletionMutex);

		fence->SetEventOnCompletion(index, waitBufferCompletionEvent);
		WaitForSingleObject(waitBufferCompletionEvent, INFINITE);
	}
}
