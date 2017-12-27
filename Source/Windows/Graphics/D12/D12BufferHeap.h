#pragma once

#include <Windows\Graphics\D12\D12Common.h>
#include <Windows\Graphics\D12\D12DescriptorHeap.h>
#include <Tools\Collections\List.h>

class D12BufferHeap
{
public:
	D12BufferHeap(ID3D12Device* device, size_t capacity);
	D3D12_GPU_VIRTUAL_ADDRESS GetOffset(const D12HeapMemory& memory) const;
	D12HeapMemory Allocate(size_t size);
	void Deallocate(D12HeapMemory& memory);

private:
	void Grow(size_t capacity);
	D12UnusedHeapMemory* FindMemory(size_t size);
	inline void Connect(D12UnusedHeapMemory* first, D12UnusedHeapMemory* second);

private:
	D12UnusedHeapMemory* begin;
	D12UnusedHeapMemory* end;
	AUTOMATED_PROPERTY_GET(ID3D12Resource*, heap);
	ID3D12Device* device;
	size_t capacity;
};