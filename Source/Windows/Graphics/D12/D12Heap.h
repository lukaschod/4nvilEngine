#pragma once

#include <Windows\Graphics\D12\D12Common.h>
#include <Tools\Collections\List.h>

enum D12HeapType
{
	D12HeapTypeSRVs,
	D12HeapTypeSamplers,
	D12HeapTypeRTVs,
	D12HeapTypeSRVsCPU,
	D12HeapTypeSamplersCPU,
	D12HeapTypeCount,
};

struct D12HeapMemory
{
	D12HeapMemory() : 
		D12HeapMemory(0, 0) 
	{
	}
	D12HeapMemory(uint64_t pointer, size_t size) :
		pointer(pointer),
		size(size)
	{
	}

	uint64_t pointer;
	size_t size;
};

struct D12UnusedHeapMemory
{
	D12UnusedHeapMemory(uint64_t pointer, size_t size) :
		pointer(pointer),
		size(size),
		next(nullptr),
		previous(nullptr)
	{
	}

	uint64_t pointer;
	size_t size;
	D12UnusedHeapMemory* previous;
	D12UnusedHeapMemory* next;
};

class D12Heap
{
public:
	D12Heap(ID3D12Device* device, D12HeapType type, size_t capacity);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(const D12HeapMemory& memory) const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(const D12HeapMemory& memory) const;
	D12HeapMemory Allocate(size_t size);
	void Free(D12HeapMemory& memory);

private:
	void Grow(size_t capacity);
	D12UnusedHeapMemory* FindMemory(size_t size);
	inline void Connect(D12UnusedHeapMemory* first, D12UnusedHeapMemory* second);

private:
	D12UnusedHeapMemory* begin;
	D12UnusedHeapMemory* end;
	AUTOMATED_PROPERTY_GET(D12HeapType, type);
	AUTOMATED_PROPERTY_GET(ID3D12DescriptorHeap*, heap);
	ID3D12Device* device;
	size_t capacity;
	size_t stride;
};