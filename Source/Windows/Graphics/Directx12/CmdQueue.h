#pragma once

#include <queue>
#include <Tools\Enum.h>
#include <Tools\IO\MemoryStream.h>
#include <Windows\Graphics\Directx12\Common.h>
#include <Windows\Graphics\Directx12\DescriptorHeap.h>
#include <Windows\Graphics\Directx12\CmdAllocatorPool.h>

using namespace Core;

namespace Windows::Directx12
{
	class CmdQueue;

	struct CmdBuffer
	{
		CmdBuffer(CmdQueue* queue, D3D12_COMMAND_LIST_TYPE type)
			: type(type)
			, queue(queue)
			, commandList(nullptr)
		{
		}

		void Reset(int64_t index)
		{
			commandCount = 0;
			this->index = index;
			swapChain = nullptr;
			stream.Reset();
			memset(heaps, 0, Enum::ToUnderlying(HeapType::Count) * sizeof(DescriptorHeap*));
		}

		const D3D12_COMMAND_LIST_TYPE type;
		CmdQueue* const queue;
		IO::MemoryStream stream;
		size_t commandCount;
		ID3D12GraphicsCommandList* commandList;
		IDXGISwapChain* swapChain;
		uint64_t index;
		DescriptorHeap* heaps[Enum::ToUnderlying(HeapType::Count)];
	};

	class CmdQueue
	{
	public:
		CmdQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
		~CmdQueue();

		CmdBuffer* Pull();
		void Push(CmdBuffer* buffer);
		void Reset(CmdBuffer* buffer, ID3D12CommandAllocator* allocator);
		void Close(CmdBuffer* buffer);
		void Execute(CmdBuffer* buffer, bool isLast);

		uint64_t GetCompletedBufferIndex();
		void WaitForBufferIndexToComplete(uint64_t index);

	private:
		const D3D12_COMMAND_LIST_TYPE type;
		ID3D12Device* const device;
		AUTOMATED_PROPERTY_GET(ID3D12CommandQueue*, queue);
		ID3D12Fence* fence;
		HANDLE waitBufferCompletionEvent;
		std::mutex waitBufferCompletionMutex;

		List<CmdBuffer*> buffers;
		std::queue<CmdBuffer*> readyBuffers;
		uint64_t pulledBufferCount;

		List<ID3D12CommandList*> cmdsToExecute;
	};
}