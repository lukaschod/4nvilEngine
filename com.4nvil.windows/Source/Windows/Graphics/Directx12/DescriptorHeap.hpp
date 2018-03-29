/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <Core\Tools\Collections\List.hpp>
#include <Core\Tools\Collections\BuddyHeapManager.hpp>
#include <Windows\Graphics\Directx12\Common.hpp>

using namespace Core;

namespace Windows::Directx12
{
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
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;
		List<BuddyHeapManager*> heapManagers;
		List<ID3D12DescriptorHeap*> descriptorHeaps;
		ID3D12Device* device;
		size_t capacity;
		size_t alignment;
	};
}