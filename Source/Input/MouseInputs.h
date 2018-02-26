#pragma once

#include <Tools\Math\Vector.h>

namespace Core
{
	struct IView;

	enum class MouseButtonType
	{
		Left,
		Right,
		Center,
		Count,
	};

	enum class MouseButtonState
	{
		None,
		Down,
		Click,
		Up,
	};

	struct MouseButtonDesc
	{
		MouseButtonType type;
		MouseButtonState state;
	};

	struct MousePositionDesc
	{
		Math::Vector2f position;
		const IView* onView;
	};
}