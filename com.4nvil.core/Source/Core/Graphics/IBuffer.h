#pragma once

#include <Core\Tools\Common.h>
#include <Core\Tools\Enum.h>

namespace Core::Graphics
{
	enum class BufferUsageFlags
	{
		None = 0,
		Shader = 1 << 0,
		CpuToGpu = 1 << 1,
		GpuOnly = 1 << 2,
		GpuToCpu = 1 << 3,
	};
	IMPLEMENT_ENUM_FLAG(BufferUsageFlags);

	struct IBuffer
	{
		IBuffer(size_t size) 
			: data(new uint8[size], size)
			, usage(BufferUsageFlags::Shader | BufferUsageFlags::CpuToGpu)
		{
		}

		inline size_t GetSize() const { return data.size; }

		Range<uint8> data;
		BufferUsageFlags usage;
	};
}