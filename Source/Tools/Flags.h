#pragma once

#include <Tools\Common.h>

namespace Core
{
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
			return (data & (uint64) flag) != 0;
		}

		inline void Add(EnumType flag)
		{
			data |= (uint64) flag;
		}

		inline void Remove(EnumType flag)
		{
			data &= !(uint64) flag;
		}

		inline operator EnumType()
		{
			return (EnumType) data;
		}

	private:
		uint64 data;
	};
}