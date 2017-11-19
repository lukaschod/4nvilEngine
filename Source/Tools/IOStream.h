#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <stdlib.h>
#include <cstring>

class IOStream
{
public:
	IOStream(size_t capacity = 128)
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

	void Reset()
	{
		data = begin;
	}

private:
	inline void MakeSureHaveSpace(size_t size)
	{
		auto totalOffset = data + size;
		if (totalOffset > end)
		{
			auto capacity = data - begin;
			auto requiredCapacity = totalOffset - begin;
			end = begin = data = (uint8_t*) realloc(begin, requiredCapacity);
			data += capacity;
			end += requiredCapacity;
		}
	}

private:
	uint8_t* begin;
	uint8_t* end;
	AUTOMATED_PROPERTY_GET(uint8_t*, data);
};

class IOStream2
{
public:
	IOStream2(size_t capacity = 128)
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
		T& ptr = *(T*) (data + offset);
		offset += sizeof(T);
		return ptr;
	}

	inline void Reset()
	{
		offset = 0;
	}

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