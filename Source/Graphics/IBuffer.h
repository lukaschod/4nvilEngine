#pragma once

#include <Tools\Common.h>

namespace Core::Graphics
{
	enum class BufferUsageFlags
	{
		None = 0,
		Shader = 1 << 0,
	};

	struct IBuffer
	{
		IBuffer(size_t size) :
			data(new uint8[size], size),
			usage(BufferUsageFlags::Shader)
		{
		}

		inline size_t GetSize() const { return data.size; }

		Range<uint8> data;
		BufferUsageFlags usage;
	};
}