#pragma once

#include <queue>
#include <mutex>
#include <Windows\Graphics\Directx12\Common.hpp>

namespace Windows::Directx12
{
	// Taken from sample and refactored to project syntax
	class CmdAllocatorPool
	{
	public:
		CmdAllocatorPool(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
		~CmdAllocatorPool();

		ID3D12CommandAllocator* TryPull(uint64 completedFenceValue);
		void Push(uint64 fenceValue, ID3D12CommandAllocator* allocator);

	private:
		const D3D12_COMMAND_LIST_TYPE type;
		ID3D12Device* const device;
		std::vector<ID3D12CommandAllocator*> allocatorPool;
		std::queue<std::pair<uint64, ID3D12CommandAllocator*>> readyAllocators;
		std::mutex allocatorMutex;
	};
}