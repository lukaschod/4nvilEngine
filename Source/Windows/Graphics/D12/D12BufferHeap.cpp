#include <Windows\Graphics\D12\D12BufferHeap.h>
#include <Tools\Math\Math.h>

D12BufferHeap::D12BufferHeap(ID3D12Device* device, size_t capacity, size_t alignment)
	: device(device)
	, alignment(alignment)
{
	Grow(capacity);
}

D12BufferHeap::~D12BufferHeap()
{
	for (int i = 0; i < heapManagers.size(); i++)
	{
		delete heapManagers[i];
		resources[i]->Release();
	}
}

D3D12_GPU_VIRTUAL_ADDRESS D12BufferHeap::GetVirtualAddress(const HeapMemory& memory) const
{
	auto index = FindIndex(memory);
	return resources[index]->GetGPUVirtualAddress() + memory.address - heapManagers[index]->GetBounds().address;
}

ID3D12Resource* D12BufferHeap::GetResource(const HeapMemory& memory) const
{
	auto index = FindIndex(memory);
	return resources[index];
}

uint8_t* D12BufferHeap::GetResourceMappedPointer(const HeapMemory& memory) const
{
	auto index = FindIndex(memory);
	return resourceMappedPointers[index];
}

size_t D12BufferHeap::GetOffset(const HeapMemory& memory) const
{
	auto index = FindIndex(memory);
	return memory.address - heapManagers[index]->GetBounds().address;
}

HeapMemory D12BufferHeap::Allocate(size_t size)
{
	size = Math::GetPadded(size, alignment);
	while (true)
	{
		auto heapManager = heapManagers.back();
		auto address = heapManager->Allocate(size);
		if (address.size == 0)
		{
			Grow(capacity);
			continue;
		}
		return address;
	}
}

void D12BufferHeap::Deallocate(const HeapMemory& memory)
{
	for (auto heapManager : heapManagers)
		if (heapManager->Deallocate(memory))
			return;
	ERROR("Invalid HeapMemory");
}

void D12BufferHeap::Grow(size_t capacity)
{
	ID3D12Resource* resource;
	ASSERT_SUCCEEDED(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(capacity),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)));
	resources.push_back(resource);

	// TODO: Clean it a bit, currently lets keep it that way to check if we win anything by always keeping resource mapped
	uint8_t* resourceMappedPointer;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	ASSERT_SUCCEEDED(resource->Map(0, &readRange, reinterpret_cast<void**>(&resourceMappedPointer)));
	resourceMappedPointers.push_back(resourceMappedPointer);

	heapManagers.push_back(new BuddyHeapManager(HeapMemory(this->capacity, capacity)));
	this->capacity += capacity;
}

int D12BufferHeap::FindIndex(const HeapMemory& memory) const
{
	for (int i = 0; i < heapManagers.size(); i++)
	{
		if (heapManagers[i]->Contains(memory))
			return i;
	}

	ERROR("Invalid HeapMemory");
	return -1;
}
