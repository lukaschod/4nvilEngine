#pragma once

#include <Core\Tools\Common.h>

namespace Core
{
	enum class ViewInputType
	{
		Render,
		Resize,
	};

	struct ViewInputResizeDesc
	{
		uint32 width;
		uint32 height;
	};
}