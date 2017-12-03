#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <Tools\Math\Math.h>

template<class T>
class HeapRow
{
public:
	T Allocate()
	{

	}

	void Free(T&& item)
	{

	}

private:
	List<Range<T>> arrays;
	List<Range<bool>> arraysState;
};