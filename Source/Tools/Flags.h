#pragma once

#include <Tools\Common.h>

// TODO: Technical depth
template<typename EnumType>
class Flags
{
public:
	Flags()
	{
		data = 0;
	}

	inline bool Contains(EnumType flag) const
	{
		return (data & (uint64_t) flag) != 0;
	}

	inline void Add(EnumType flag)
	{
		data |= (uint64_t) flag;
	}

	inline void Remove(EnumType flag)
	{
		data &= !(uint64_t)flag;
	}

	inline operator EnumType()
	{
		return (EnumType)data;
	}

private:
	uint64_t data;
};