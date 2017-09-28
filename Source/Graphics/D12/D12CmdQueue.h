#pragma once

#include <Graphics\D12\D12Common.h>
#include <Graphics\D12\D12Heap.h>
#include <Graphics\D12\D12CmdAllocatorPool.h>
#include <Common\IOStream.h>
#include <queue>

class D12CmdQueue;

struct D12CmdBuffer
{
	D12CmdBuffer(D12CmdQueue* queue, D3D12_COMMAND_LIST_TYPE type) :
		type(type),
		queue(queue),
		commandList(nullptr)
	{
	}

	void Reset(int64_t index)
	{
		commandCount = 0;
		this->index = index;
		swapChain = nullptr;
		stream.Reset();
		memset(heaps, 0, kD12HeapTypeCount * sizeof(D12Heap*));
	}

	const D3D12_COMMAND_LIST_TYPE type;
	D12CmdQueue* const queue;
	IOStream stream;
	size_t commandCount;
	ID3D12GraphicsCommandList* commandList;
	IDXGISwapChain* swapChain;
	uint64_t index;
	D12Heap* heaps[kD12HeapTypeCount];
};

class D12CmdQueue
{
public:
	D12CmdQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	~D12CmdQueue();

	D12CmdBuffer* Pull();
	void Push(D12CmdBuffer* buffer);
	void Reset(D12CmdBuffer* buffer, ID3D12CommandAllocator* allocator);
	void Close(D12CmdBuffer* buffer);
	void Execute(D12CmdBuffer* buffer);

	uint64_t GetCompletedBufferIndex();
	void WaitForBufferIndexToComplete(uint64_t index);

private:
	const D3D12_COMMAND_LIST_TYPE type;
	ID3D12Device* const device;
	AUTOMATED_PROPERTY_GET(ID3D12CommandQueue*, queue);
	ID3D12Fence* fence;
	HANDLE waitBufferCompletionEvent;
	std::mutex waitBufferCompletionMutex;

	List<D12CmdBuffer*> buffers;
	std::queue<D12CmdBuffer*> readyBuffers;
	D12CmdAllocatorPool* allocatorPool;
	uint64_t pulledBufferCount;
};