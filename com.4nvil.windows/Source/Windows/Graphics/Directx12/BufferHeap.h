#pragma once

#include <Core\Tools\Collections\List.h>
#include <Core\Tools\Collections\BuddyHeapManager.h>
#include <Windows\Graphics\Directx12\Common.h>
#include <Windows\Graphics\Directx12\DescriptorHeap.h>

using namespace Core;

namespace Windows::Directx12
{
	class BufferHeap
	{
	public:
		BufferHeap(ID3D12Device* device, size_t capacity, size_t alignment, D3D12_HEAP_TYPE type);
		~BufferHeap();
		HeapMemory Allocate(size_t size);
		void Deallocate(const HeapMemory& memory);
		D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress(const HeapMemory& memory) const;
		ID3D12Resource* GetResource(const HeapMemory& memory) const;
		uint8* GetResourceMappedPointer(const HeapMemory& memory) const;
		D3D12_RESOURCE_STATES* GetState(const HeapMemory& memory) const;
		uint64 GetResourceOffset(const HeapMemory& memory) const;

	private:
		void Grow(size_t capacity);
		int FindIndex(const HeapMemory& memory) const;

	private:
		D3D12_HEAP_TYPE type;
		List<BuddyHeapManager*> heapManagers;
		List<ID3D12Resource*> resources;
		List<D3D12_RESOURCE_STATES> states;
		List<uint8*> resourceMappedPointers;
		ID3D12Device* device;
		size_t capacity;
		size_t alignment;
	};
}