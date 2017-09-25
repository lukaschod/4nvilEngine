// Taken from sample and refactored to project syntax

#pragma once

#include <Graphics\D12\D12Common.h>
#include <queue>
#include <mutex>

class D12CmdAllocatorPool
{
public:
	D12CmdAllocatorPool(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	~D12CmdAllocatorPool();

	ID3D12CommandAllocator* TryPull(uint64_t completedFenceValue);
	void Push(uint64_t fenceValue, ID3D12CommandAllocator* allocator);

private:
	const D3D12_COMMAND_LIST_TYPE type;

	ID3D12Device* const device;
	std::vector<ID3D12CommandAllocator*> allocatorPool;
	std::queue<std::pair<uint64_t, ID3D12CommandAllocator*>> readyAllocators;
	std::mutex allocatorMutex;
};