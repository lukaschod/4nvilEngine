#include <Windows\Graphics\D12\D12DescriptorHeap.h>

D12DescriptorHeap::D12DescriptorHeap(ID3D12Device* device, D12HeapType type, size_t capacity) :
	device(device),
	type(type)
{
	Grow(capacity);
}

D3D12_GPU_DESCRIPTOR_HANDLE D12DescriptorHeap::GetGpuHandle(const D12HeapMemory& memory) const
{
	auto handle = heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += stride * memory.pointer;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE D12DescriptorHeap::GetCpuHandle(const D12HeapMemory& memory) const
{
	auto handle = heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += stride * memory.pointer;
	return handle;
}

D12HeapMemory D12DescriptorHeap::Allocate(size_t size)
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

void D12DescriptorHeap::Deallocate(D12HeapMemory& memory)
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

void D12DescriptorHeap::Grow(size_t capacity)
{
	if (heap == nullptr)
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = (UINT)capacity;
		switch (type)
		{
		case D12HeapTypeSRVs:
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			stride = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
			break;
		case D12HeapTypeSamplers:
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			stride = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
			break;
		case D12HeapTypeRTVs:
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			stride = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
			break;
		case D12HeapTypeSRVsCPU:
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			stride = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
			break;
		case D12HeapTypeSamplersCPU:
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			stride = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);
			break;
		}
		
		ASSERT_SUCCEEDED((device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&heap))));

		begin = new D12UnusedHeapMemory(0, 0);
		end = new D12UnusedHeapMemory(UINT64_MAX - 1, 0);

		auto memory = new D12UnusedHeapMemory(1, capacity);
		memory->previous = begin;
		memory->next = end;
		begin->next = memory;
		end->previous = memory;

		this->capacity = capacity;
		return;
	}

	ERROR("NotImplemented");
}

D12UnusedHeapMemory* D12DescriptorHeap::FindMemory(size_t size)
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

void D12DescriptorHeap::Connect(D12UnusedHeapMemory* first, D12UnusedHeapMemory* second)
{
	first->next = second;
	second->previous = first;
}
