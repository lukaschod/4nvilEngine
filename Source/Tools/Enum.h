#pragma once

#include <type_traits>

namespace Core::Enum
{
	template<typename E>
	constexpr typename std::underlying_type<E>::type ToUnderlying(E e) noexcept
	{
		return static_cast<typename std::underlying_type<E>::type>(e);
	}
}