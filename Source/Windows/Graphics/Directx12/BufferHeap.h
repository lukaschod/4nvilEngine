#pragma once

#include <Tools\Collections\List.h>
#include <Tools\Collections\BuddyHeapManager.h>
#include <Windows\Graphics\Directx12\Common.h>
#include <Windows\Graphics\Directx12\DescriptorHeap.h>

using namespace Core;

namespace Windows::Directx12
{
	class BufferHeap
	{
	public:
		BufferHeap(ID3D12Device* device, size_t capacity, size_t alignment);
		~BufferHeap();
		HeapMemory Allocate(size_t size);
		void Deallocate(const HeapMemory& memory);
		D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress(const HeapMemory& memory) const;
		ID3D12Resource* GetResource(const HeapMemory& memory) const;
		uint8* GetResourceMappedPointer(const HeapMemory& memory) const;

	private:
		void Grow(size_t capacity);
		int FindIndex(const HeapMemory& memory) const;

	private:
		List<BuddyHeapManager*> heapManagers;
		List<ID3D12Resource*> resources;
		List<uint8*> resourceMappedPointers;
		ID3D12Device* device;
		size_t capacity;
		size_t alignment;
	};
}