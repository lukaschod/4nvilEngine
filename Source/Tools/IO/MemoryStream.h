#pragma once

#include <Tools\Common.h>
#include <stdlib.h>

class MemoryStream
{
public:
	MemoryStream(size_t capacity = 128)
	{
		end = begin = data = (uint8_t*) malloc(capacity);
		end += capacity;
	}

	template<typename T>
	inline void Write(T& data)
	{
		Write((void*) &data, sizeof(T));
	}

	inline void Write(uint32_t v)
	{
		Write((void*) &v, sizeof(uint32_t));
	}

	inline void Write(void* data, size_t size)
	{
		MakeSureHaveSpace(size); 
		memcpy(this->data, data, size);
		this->data += size;
	}

	template<typename T>
	inline void Read(T& data)
	{
		Read((void*) &data, sizeof(T));
	}

	inline void Read(void* data, size_t size)
	{
		memcpy(data, this->data, size);
		this->data += size;
	}

	template<typename T>
	inline T& FastRead()
	{
		T& ptr = *(T*) data;
		this->data += sizeof(T);
		return ptr;
	}

	inline void Reset()
	{
		data = begin;
	}

	inline size_t GetCapacity()
	{
		return end - begin;
	}

	inline void Align(size_t padding = sizeof(void*))
	{
		data += (size_t)data % padding;
		ASSERT(data <= end);
	}

private:
	inline void MakeSureHaveSpace(size_t size)
	{
		while (data + size > end)
		{
			auto capacity = data - begin;
			auto requiredCapacity = (end - begin) * 2;
			begin = (uint8_t*) realloc(begin, requiredCapacity);
			data = begin + capacity;
			end = begin + requiredCapacity;
		}
	}

private:
	uint8_t* begin;
	uint8_t* end;
	AUTOMATED_PROPERTY_GETSET(uint8_t*, data);
};