#pragma once

#include <Tools\Collections\List.h>
#include <Tools\Collections\BuddyHeapManager.h>
#include <Windows\Graphics\Directx12\Common.h>

namespace Windows::Directx12
{
	using namespace Core;

	enum class HeapType
	{
		SRVs,
		Samplers,
		RTVs,
		SRVsCPU,
		SamplersCPU,
		Count,
	};

	class DescriptorHeap
	{
	public:
		DescriptorHeap(ID3D12Device* device, HeapType type, size_t capacity);
		~DescriptorHeap();
		HeapMemory Allocate(size_t size);
		void Deallocate(const HeapMemory& memory);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(const HeapMemory& memory) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(const HeapMemory& memory) const;

		ID3D12DescriptorHeap* GetDesciptorHeap() const { return descriptorHeaps.front(); }

	private:
		void Grow(size_t capacity);
		int FindIndex(const HeapMemory& memory) const;

	private:
		AUTOMATED_PROPERTY_GET(HeapType, type);
		List<BuddyHeapManager*> heapManagers;
		List<ID3D12DescriptorHeap*> descriptorHeaps;
		ID3D12Device* device;
		size_t capacity;
		size_t alignment;
	};
}