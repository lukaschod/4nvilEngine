#pragma once

#include <Windows\Graphics\D12\D12Common.h>
#include <Windows\Graphics\D12\D12DescriptorHeap.h>
#include <Windows\Graphics\D12\D12CmdAllocatorPool.h>
#include <Tools\IO\MemoryStream.h>
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
		memset(heaps, 0, D12HeapTypeCount * sizeof(D12DescriptorHeap*));
	}

	const D3D12_COMMAND_LIST_TYPE type;
	D12CmdQueue* const queue;
	MemoryStream stream;
	size_t commandCount;
	ID3D12GraphicsCommandList* commandList;
	IDXGISwapChain* swapChain;
	uint64_t index;
	D12DescriptorHeap* heaps[D12HeapTypeCount];
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
	void Execute(D12CmdBuffer* buffer, bool isLast);

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
	uint64_t pulledBufferCount;

	List<ID3D12CommandList*> cmdsToExecute;
};