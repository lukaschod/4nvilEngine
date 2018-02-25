#pragma once

#include <Windows\Graphics\D12\D12Common.h>
#include <Windows\Graphics\D12\D12DescriptorHeap.h>
#include <Tools\Collections\List.h>
#include <Tools\Collections\BuddyHeapManager.h>

class D12BufferHeap
{
public:
	D12BufferHeap(ID3D12Device* device, size_t capacity, size_t alignment);
	~D12BufferHeap();
	
	HeapMemory Allocate(size_t size);
	void Deallocate(const HeapMemory& memory);
	D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress(const HeapMemory& memory) const;
	ID3D12Resource* GetResource(const HeapMemory& memory) const;
	uint8_t* GetResourceMappedPointer(const HeapMemory& memory) const;

private:
	void Grow(size_t capacity);
	int FindIndex(const HeapMemory& memory) const;

private:
	List<BuddyHeapManager*> heapManagers;
	List<ID3D12Resource*> resources;
	List<uint8_t*> resourceMappedPointers;
	ID3D12Device* device;
	size_t capacity;
	size_t alignment;
};