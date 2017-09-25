#pragma once

#include <Common\EngineCommon.h>

enum BufferUsageFlags
{
	kBufferUsageFlagNone = 0,
	kBufferUsageFlagsShader = 1 << 0,
};

struct IBuffer
{
	IBuffer(size_t size) : 
		data(new uint8_t[size], size),
		usage(kBufferUsageFlagsShader)
	{}

	inline size_t GetSize() const { return data.size; }

	Range<uint8_t> data;
	BufferUsageFlags usage;
};