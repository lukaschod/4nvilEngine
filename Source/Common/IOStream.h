#pragma once

#include <Common\EngineCommon.h>
#include <Common\Collections\List.h>
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

	inline void Write(uint32_t v)
	{
		Write((void*) &v, sizeof(uint32_t));
	}

	inline void Write(void* data, size_t size)
	{
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
		memcpy(data, this->data + offset, size);
		offset += size;
	}

	template<typename T>
	inline T& FastRead()
	{
		T& ptr = *(T*) (data + sizeof(T));
		offset += sizeof(T);
		return ptr;
	}

	inline void Cleanup()
	{
		size = 0;
	}

	inline bool IsFull() { return size == offset; }

private:
	inline void MakeSureHaveSpace(size_t size)
	{
		auto totalOffset = offset + size;
		this->size = totalOffset;
		if (totalOffset > capacity)
		{
			capacity = totalOffset;
			ASSERT(capacity < 500000);
			data = (uint8_t*) realloc(data, capacity);
		}
	}

private:
	uint8_t* data;
	AUTOMATED_PROPERTY_GET(size_t, size);
	AUTOMATED_PROPERTY_GET(size_t, capacity);
	AUTOMATED_PROPERTY_GETSET(size_t, offset);
};