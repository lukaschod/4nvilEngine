#pragma once

#include <Common\EngineCommon.h>
#include <stdlib.h>
#include <cstring>

class IOStream
{
public:
	IOStream(size_t capacity = 128)
	{
		data = (uint8_t*)malloc(capacity);
		offset = 0;
		size = 0;
		this->capacity = capacity;
	}

	template<typename T>
	inline void Write(T& data)
	{
		Write((void*)&data, sizeof(T));
	}

	inline void Write(void* data, size_t size)
	{
		DebugAssert(data != nullptr);
		DebugAssert(size != 0);
		MakeSureHaveSpace(size);
		memcpy(this->data + offset, data, size);
		offset += size;
	}

	template<typename T>
	inline void Read(T& data)
	{
		Read((void*)&data, sizeof(T));
	}

	inline void Read(void* data, size_t size)
	{
		DebugAssert(data != nullptr);
		DebugAssert(size != 0);
		memcpy(data, this->data + offset, size);
		offset += size;
	}

	inline void Cleanup()
	{
		size = 0;
	}

private:
	inline void MakeSureHaveSpace(size_t size)
	{
		auto totalOffset = offset + size;
		if (totalOffset > capacity)
		{
			data = (uint8_t*) realloc(data, totalOffset);
			capacity = totalOffset;
			this->size = totalOffset;
		}
	}

private:
	uint8_t* data;
	AUTOMATED_PROPERTY_GET(size_t, size);
	AUTOMATED_PROPERTY_GET(size_t, capacity);
	AUTOMATED_PROPERTY_GETSET(uint32_t, offset);
};