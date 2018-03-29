/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <Core\Tools\Common.hpp>

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