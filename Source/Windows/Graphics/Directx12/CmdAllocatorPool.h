#pragma once

#include <queue>
#include <mutex>
#include <Windows\Graphics\Directx12\Common.h>

namespace Windows::Directx12
{
	// Taken from sample and refactored to project syntax
	class CmdAllocatorPool
	{
	public:
		CmdAllocatorPool(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
		~CmdAllocatorPool();

		ID3D12CommandAllocator* TryPull(uint64_t completedFenceValue);
		void Push(uint64_t fenceValue, ID3D12CommandAllocator* allocator);

	private:
		const D3D12_COMMAND_LIST_TYPE type;
		ID3D12Device* const device;
		std::vector<ID3D12CommandAllocator*> allocatorPool;
		std::queue<std::pair<uint64_t, ID3D12CommandAllocator*>> readyAllocators;
		std::mutex allocatorMutex;
	};
}