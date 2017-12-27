#include "D12CmdAllocatorPool.h"

D12CmdAllocatorPool::D12CmdAllocatorPool(ID3D12Device * device, D3D12_COMMAND_LIST_TYPE type)
	: device(device)
	, type(type)
{
}

D12CmdAllocatorPool::~D12CmdAllocatorPool()
{
	for (auto allocator : allocatorPool)
		allocator->Release();
	allocatorPool.clear();
}

ID3D12CommandAllocator* D12CmdAllocatorPool::TryPull(uint64_t completedFenceValue)
{
	std::lock_guard<std::mutex> lock(allocatorMutex);

	ID3D12CommandAllocator* allocator = nullptr;

	if (!readyAllocators.empty())
	{
		std::pair<uint64_t, ID3D12CommandAllocator*>& allocatorPair = readyAllocators.front();

		if (allocatorPair.first <= completedFenceValue)
		{
			allocator = allocatorPair.second;
			ASSERT_SUCCEEDED(allocator->Reset());
			readyAllocators.pop();
		}
	}

	// If no allocator's were ready to be reused, create a new one
	if (allocator == nullptr)
	{
		ASSERT_SUCCEEDED(device->CreateCommandAllocator(type, IID_PPV_ARGS(&allocator)));
		wchar_t allocatorName[32];
		swprintf(allocatorName, 32, L"CommandAllocator %zu", allocatorPool.size());
		allocator->SetName(allocatorName);
		allocatorPool.push_back(allocator);
	}

	return allocator;
}

void D12CmdAllocatorPool::Push(uint64_t fenceValue, ID3D12CommandAllocator* allocator)
{
	std::lock_guard<std::mutex> lock(allocatorMutex);

	// That fence value indicates we are free to reset the allocator
	readyAllocators.push(std::make_pair(fenceValue, allocator));
}
