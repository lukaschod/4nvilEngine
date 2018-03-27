#include <Windows/Graphics/Directx12/CmdQueue.h>

using namespace Windows::Directx12;

CmdQueue::CmdQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
	: device(device)
	, type(type)
	, pulledBufferCount(0)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = type;
	queueDesc.NodeMask = 1;
	ASSERT_SUCCEEDED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue)));
	
	ASSERT_SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

	waitBufferCompletionEvent = CreateEvent(nullptr, false, false, nullptr);
	ASSERT_SUCCEEDED(waitBufferCompletionEvent != INVALID_HANDLE_VALUE);
}

CmdQueue::~CmdQueue()
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

CmdBuffer* CmdQueue::Pull()
{
	if (readyBuffers.empty())
	{
		auto readyBuffer = new CmdBuffer(this, type);
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

void CmdQueue::Push(CmdBuffer * buffer)
{
	readyBuffers.push(buffer);
}

void CmdQueue::Reset(CmdBuffer* buffer, ID3D12CommandAllocator* allocator)
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

void CmdQueue::Close(CmdBuffer* buffer)
{
	buffer->commandList->Close();
}

void CmdQueue::Execute(CmdBuffer* buffer, bool isLast)
{
	if (buffer->swapChain != nullptr)
	{
		if (!cmdsToExecute.empty())
		{
			queue->ExecuteCommandLists((UINT) cmdsToExecute.size(), cmdsToExecute.data());
			//queue->Signal(fence, buffer->index);
			//queue->ExecuteCommandLists(1/*cmds.size()*/, cmds.data() + 1);
			cmdsToExecute.clear();
		}

		buffer->swapChain->Present(0, 0);

		if (isLast)
			queue->Signal(fence, buffer->index);
	}
	else
	{
		cmdsToExecute.push_back(buffer->commandList);

		if (isLast)
		{
			queue->ExecuteCommandLists((UINT) cmdsToExecute.size(), cmdsToExecute.data());
			cmdsToExecute.clear();
			queue->Signal(fence, buffer->index);
		}

		//ASSERT_SUCCEEDED(buffer->commandList != nullptr);
		//queue->ExecuteCommandLists(1, (ID3D12CommandList**) &buffer->commandList);
		//queue->Signal(fence, buffer->index);
	}
}

uint64 CmdQueue::GetCompletedBufferIndex()
{
	// TODO: Sync?
	return fence->GetCompletedValue();
}

void CmdQueue::WaitForBufferIndexToComplete(uint64 index)
{
	if (index <= fence->GetCompletedValue())
		return;

	{
		std::lock_guard<std::mutex> lock(waitBufferCompletionMutex);

		fence->SetEventOnCompletion(index, waitBufferCompletionEvent);
		WaitForSingleObject(waitBufferCompletionEvent, INFINITE);
	}
}