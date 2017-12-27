#include <Windows\Graphics\D12\D12BufferHeap.h>

D12BufferHeap::D12BufferHeap(ID3D12Device* device, size_t capacity) 
	: device(device)
{
	Grow(capacity);
}

D3D12_GPU_VIRTUAL_ADDRESS D12BufferHeap::GetOffset(const D12HeapMemory& memory) const
{
	return heap->GetGPUVirtualAddress() + memory.pointer;
}

D12HeapMemory D12BufferHeap::Allocate(size_t size)
{
	auto unusedMemory = FindMemory(size);
	if (unusedMemory == end)
	{
		Grow(capacity * 2);
		return Allocate(size);
	}

	// Shrink the unused memory
	if (unusedMemory->size > size)
	{
		auto allocationPointer = unusedMemory->pointer;
		unusedMemory->size -= size;
		unusedMemory->pointer += size;
		return D12HeapMemory(allocationPointer, size);
	}

	// Remove block
	Connect(unusedMemory->previous, unusedMemory->next);
	delete unusedMemory;
	return D12HeapMemory(unusedMemory->pointer, unusedMemory->size);
}

void D12BufferHeap::Deallocate(D12HeapMemory& memory)
{
	auto current = begin;
	while (current != end)
	{
		auto next = current->next;
		auto mergableWithBlocAbove = current->pointer + current->size == memory.pointer;
		auto mergableWithBlocBelow = next->pointer == memory.pointer + memory.size;
		
		// 1) Fred memory can be merged with block above
		if (mergableWithBlocAbove && !mergableWithBlocBelow)
		{
			current->size += memory.size;
			return;
		}

		// 2) Fred memory can be merged with block below
		if (!mergableWithBlocAbove && mergableWithBlocBelow)
		{
			next->pointer -= memory.size;
			return;
		}

		// 3) Fred memory can be merged with blocks above and below
		if (mergableWithBlocAbove && mergableWithBlocBelow)
		{
			current->size += memory.size + next->size;
			Connect(current, next->next);
			delete next;
			return;
		}

		// 4) Fred memory needs new block
		if (current->pointer + current->size < memory.pointer && next->pointer > memory.pointer + memory.size)
		{
			auto created = new D12UnusedHeapMemory(memory.pointer, memory.size);
			Connect(current, created);
			Connect(created, next);
			return;
		}

		current = next;
	}

	ERROR("Memory is corrupted");
}

void D12BufferHeap::Grow(size_t capacity)
{
	if (heap == nullptr)
	{
		ASSERT_SUCCEEDED(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(capacity),
				D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&heap)));

		begin = new D12UnusedHeapMemory(0, 0);
		end = new D12UnusedHeapMemory(UINT64_MAX - 1, 0);

		auto memory = new D12UnusedHeapMemory(0, capacity);
		memory->previous = begin;
		memory->next = end;
		begin->next = memory;
		end->previous = memory;

		this->capacity = capacity;
		return;
	}

	ERROR("NotImplemented");
}

D12UnusedHeapMemory* D12BufferHeap::FindMemory(size_t size)
{
	D12UnusedHeapMemory* iterator = begin;
	while (iterator != end)
	{
		if (iterator->size > size)
			return iterator;

		iterator = iterator->next;
	}
	return end;
}

void D12BufferHeap::Connect(D12UnusedHeapMemory* first, D12UnusedHeapMemory* second)
{
	first->next = second;
	second->previous = first;
}
