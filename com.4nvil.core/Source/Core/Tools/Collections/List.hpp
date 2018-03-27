#pragma once

#include <Core\Tools\Common.hpp>
#include <vector>

namespace Core
{
	template<class TypeName>
	class Iterator
	{
	public:
		Iterator() {}
		Iterator(TypeName* ptr) : ptr(ptr) {}

		inline TypeName& operator *() const { return *ptr; }

		inline const Iterator<TypeName>& operator+=(const Iterator<TypeName>& v) { ptr += v.ptr; return *this; }
		inline const Iterator<TypeName>& operator-=(const Iterator<TypeName>& v) { ptr -= v.ptr; return *this; }
		inline const Iterator<TypeName>& operator*=(const Iterator<TypeName>& v) { ptr *= v.ptr; return *this; }
		inline const Iterator<TypeName>& operator/=(const Iterator<TypeName>& v) { ptr /= v.ptr; return *this; }
		inline const Iterator<TypeName>& operator+=(int v) { ptr += v; return *this; }
		inline const Iterator<TypeName>& operator-=(int v) { ptr -= v; return *this; }
		inline const Iterator<TypeName>& operator*=(int v) { ptr *= v; return *this; }
		inline const Iterator<TypeName>& operator/=(int v) { ptr /= v; return *this; }

		inline bool operator==(const Iterator<TypeName>& v) const { return ptr == v.ptr; }
		inline bool operator!=(const Iterator<TypeName>& v) const { return ptr != v.ptr; }
		inline bool operator<=(const Iterator<TypeName>& v) const { return ptr <= v.ptr; }
		inline bool operator>=(const Iterator<TypeName>& v) const { return ptr >= v.ptr; }
		inline bool operator&&(const Iterator<TypeName>& v) const { return ptr && v.ptr; }
		inline bool operator||(const Iterator<TypeName>& v) const { return ptr || v.ptr; }
		inline bool operator<(const Iterator<TypeName>& v) const { return ptr < v.ptr; }
		inline bool operator>(const Iterator<TypeName>& v) const { return ptr > v.ptr; }

		inline const Iterator<TypeName>& operator++() { ptr++; return *this; }
		inline const Iterator<TypeName>& operator--() { ptr--; return *this; }

		TypeName* ptr;
	};

	template<class TypeName>
	inline const Iterator<TypeName> operator<(int first, const Iterator<TypeName>& second) { return first < second.ptr; }

	template<class TypeName>
	inline const Iterator<TypeName> operator>(int first, const Iterator<TypeName>& second) { return first > second.ptr; }

	template<class TypeName>
	inline const Iterator<TypeName> operator+(const Iterator<TypeName>& first, const Iterator<TypeName>& second)
	{
		Iterator<TypeName> o = first;
		o += second;
		return o;
	}

	template<class TypeName>
	inline const Iterator<TypeName> operator-(const Iterator<TypeName>& first, const Iterator<TypeName>& second)
	{
		Iterator<TypeName> o = first;
		o -= second;
		return o;
	}

	template<class TypeName>
	inline const Iterator<TypeName> operator+(const Iterator<TypeName>& first, int second)
	{
		Iterator<TypeName> o = first;
		o += second;
		return o;
	}

	template<class TypeName>
	inline const Iterator<TypeName> operator-(const Iterator<TypeName>& first, int second)
	{
		Iterator<TypeName> o = first;
		o -= second;
		return o;
	}

	template<class TypeName>
	class List2
	{
	public:
		List2()
		{
			reserve(1);
		}

		List2(size_t capacity)
		{
			_begin = (TypeName*) malloc(sizeof(TypeName) * capacity);
			_end = _begin + capacity;
			current = _begin;
		}

		inline TypeName& get(size_t index) const
		{
			ASSERT(_begin <= _begin + index && _begin + index <= _end);
			return _begin[index];
		}

		inline TypeName& operator[](size_t index) const
		{
			return get(index);
		}

		inline void push_back(const TypeName& item)
		{
			reserveFor(size() + 1);
			*current = item;
			current++;
		}

		inline void push_back(const TypeName&& item)
		{
			reserveFor(size() + 1);
			*current = item;
			current++;
		}

		inline void pop_back()
		{
			current--;
		}

		inline void clear()
		{
			current = _begin;
		}

		inline void resize(size_t _size)
		{
			reserveFor(size() + _size);
			current = _begin + _size;
		}

		inline void reserve(size_t capacity)
		{
			auto size2 = size();
			_begin = (TypeName*) realloc(_begin, sizeof(TypeName) *capacity);
			_end = _begin + capacity;
			current = _begin + size2;
		}

		inline void erase(Iterator<TypeName>& itr)
		{
			memcpy(itr.ptr + 1, itr.ptr, (current - itr.ptr) * sizeof(TypeName));
			current--;
		}

		/*inline Iterator<TypeName> find(TypeName&& item)
		{
			for (auto itr = begin(); itr < end(); itr++)
			{

			}
		}*/

		inline TypeName* data() const { return _begin; }
		inline bool empty() const { return current == _begin; }
		inline size_t size() const { return current - _begin; }
		inline size_t capacity() const { return _end - _begin; }
		inline Iterator<TypeName> begin() const { return Iterator<TypeName>(_begin); }
		inline Iterator<TypeName> end() const { return Iterator<TypeName>(current); }
		inline TypeName& front() const { return *_begin; }
		inline TypeName& back() const { return *_end; }

	private:
		inline void reserveFor(size_t size2)
		{
			auto capacity2 = capacity();
			if (size() + size2 >= capacity2)
				reserve(capacity2 * 2);
		}

	private:
		TypeName* _begin;
		TypeName* _end;
		TypeName* current;
	};

	template<class T>
	class List : public std::vector<T>
	{
	public:
		List() :
			std::vector<T>()
		{
		}
		List(size_t size) :
			std::vector<T>(size)
		{
		}

		void remove(T item)
		{
			auto target = std::find(begin(), end(), item);
			ASSERT(target != end());
			erase(target);
		}

		void safe_set(uint32 index, T& item)
		{
			if (size() <= index)
				resize(index + 1);

			auto target = this->begin() + index;
			*target = item;
		}

		int find(T item)
		{
			for (int i = 0; i < size(); i++)
			{
				if (this->data()[i] == item)
					return i;
			}
			return -1;
		}

		bool safe_push_back(T item)
		{
			auto index = find(item);
			if (index == -1)
			{
				push_back(item);
				return true;
			}
			return false;
		}

		void push_back_list(List<T>& list)
		{
			auto startSize = size();
			this->resize(size() + list.size());
			memcpy(data() + startSize, list.data(), list.size() * sizeof(T));
		}
	};
}