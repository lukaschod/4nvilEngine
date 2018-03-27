#pragma once

#include <stdlib.h>
#include <Core\Tools\Common.h>

namespace Core::IO
{
	class MemoryStream
	{
	public:
		MemoryStream(size_t capacity = 128)
		{
			end = begin = data = (uint8*) malloc(capacity);
			end += capacity;
		}

		template<typename T>
		inline void Write(T& data)
		{
			Write((void*) &data, sizeof(T));
		}

		inline void Write(std::string& data)
		{
			Write((void*) data.c_str(), data.size());
		}

		inline void Write(uint32 v)
		{
			Write((void*) &v, sizeof(uint32));
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

		inline std::string FastRead()
		{
			std::string out((const char*)data);
			this->data += sizeof(out.size());
			return out;
		}

		template<typename T>
		inline T& FastRead(size_t& offset) const
		{
			T& ptr = *(T*) (begin + offset);
			offset += sizeof(T);
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
			data += (size_t) data % padding;
			ASSERT(data <= end);
		}

	private:
		inline void MakeSureHaveSpace(size_t size)
		{
			while (data + size > end)
			{
				auto capacity = data - begin;
				auto requiredCapacity = (end - begin) * 2;
				begin = (uint8*) realloc(begin, requiredCapacity);
				data = begin + capacity;
				end = begin + requiredCapacity;
			}
		}

	private:
		uint8* begin;
		uint8* end;
		AUTOMATED_PROPERTY_GETSET(uint8*, data);
	};
}