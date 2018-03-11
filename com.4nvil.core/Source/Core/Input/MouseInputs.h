#pragma once

#include <Core\Tools\Math\Vector.h>

namespace Core
{
	struct IView;

	enum class MouseInputType
	{
		Button,
		Move,
	};

	enum class MouseButtonType
	{
		Left,
		Right,
		Center,
		Count,
	};

	struct MouseButtonState
	{
		bool up;
		bool down;
		bool click;
	};

	struct MouseButtonDesc
	{
		MouseButtonType type;
		bool isDown;
	};

	struct MousePositionDesc
	{
		Math::Vector2f position;
		const IView* onView;
	};
}