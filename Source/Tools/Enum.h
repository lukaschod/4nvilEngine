#pragma once

#include <type_traits>

namespace Core::Enum
{
	// Converts enum value into any primitive type
	template<typename E> constexpr typename std::underlying_type<E>::type ToUnderlying(E e) noexcept
	{
		return static_cast<typename std::underlying_type<E>::type>(e);
	}
}