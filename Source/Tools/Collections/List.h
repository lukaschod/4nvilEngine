#pragma once

#include <Tools\Common.h>
#include <vector>


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

	void safe_set(uint32_t index, T item)
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

	void safe_push_back(T item)
	{
		auto index = find(item);
		if (index == -1)
			push_back(item);
	}

	void push_back_list(List<T>& list)
	{
		auto startSize = size();
		this->resize(size() + list.size());
		memcpy(data() + startSize, list.data(), list.size() * sizeof(T));
	}
};