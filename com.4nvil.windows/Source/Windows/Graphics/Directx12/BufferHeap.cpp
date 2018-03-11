#include <Core\Tools\Math\Math.h>
#include <Windows\Graphics\Directx12\BufferHeap.h>

using namespace Windows::Directx12;

BufferHeap::BufferHeap(ID3D12Device* device, size_t capacity, size_t alignment, D3D12_HEAP_TYPE type)
	: device(device)
	, alignment(alignment)
	, type(type)
{
	Grow(capacity);
	Allocate(alignment); // Reserve for null pointer
}

BufferHeap::~BufferHeap()
{
	for (int i = 0; i < heapManagers.size(); i++)
	{
		delete heapManagers[i];
		resources[i]->Release();
	}
}

D3D12_GPU_VIRTUAL_ADDRESS BufferHeap::GetVirtualAddress(const HeapMemory& memory) const
{
	auto index = FindIndex(memory);
	return resources[index]->GetGPUVirtualAddress() + memory.address - heapManagers[index]->GetBounds().address;
}

ID3D12Resource* BufferHeap::GetResource(const HeapMemory& memory) const
{
	auto index = FindIndex(memory);
	return resources[index];
}

uint8* BufferHeap::GetResourceMappedPointer(const HeapMemory& memory) const
{
	auto index = FindIndex(memory);
	return resourceMappedPointers[index] + memory.address - heapManagers[index]->GetBounds().address;
}

D3D12_RESOURCE_STATES* BufferHeap::GetState(const HeapMemory& memory) const
{
	auto index = FindIndex(memory);
	return (D3D12_RESOURCE_STATES*)states.data() + index;
}

uint64 BufferHeap::GetResourceOffset(const HeapMemory & memory) const
{
	auto index = FindIndex(memory);
	return memory.address - heapManagers[index]->GetBounds().address;
}

HeapMemory BufferHeap::Allocate(size_t size)
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

void BufferHeap::Deallocate(const HeapMemory& memory)
{
	for (auto heapManager : heapManagers)
		if (heapManager->Deallocate(memory))
			return;
	ERROR("Invalid HeapMemory");
}

void BufferHeap::Grow(size_t capacity)
{
	auto state = D3D12_RESOURCE_STATE_GENERIC_READ;

	ID3D12Resource* resource;
	ASSERT_SUCCEEDED(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(type),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(capacity),
		state,
		nullptr,
		IID_PPV_ARGS(&resource)));
	resources.push_back(resource);

	// TODO: Clean it a bit, currently lets keep it that way to check if we win anything by always keeping resource mapped
	if (type == D3D12_HEAP_TYPE_UPLOAD)
	{
		uint8* resourceMappedPointer;
		CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
		ASSERT_SUCCEEDED(resource->Map(0, &readRange, reinterpret_cast<void**>(&resourceMappedPointer)));
		resourceMappedPointers.push_back(resourceMappedPointer);
	}

	states.push_back(state);

	heapManagers.push_back(new BuddyHeapManager(HeapMemory(this->capacity, capacity)));
	this->capacity += capacity;
}

int BufferHeap::FindIndex(const HeapMemory& memory) const
{
	for (int i = 0; i < heapManagers.size(); i++)
	{
		if (heapManagers[i]->Contains(memory))
			return i;
	}

	ERROR("Invalid HeapMemory");
	return -1;
}
