#pragma once

#include <Core\Tools\Common.h>
#include <Core\Tools\Collections\List.h>
#include <Core\Tools\Math\Color.h>

namespace Core::Graphics
{
	enum class VertexAttributeType
	{
		Position,
		TexCoord0,
	};

	struct VertexAttributeLayout
	{
		VertexAttributeLayout() {}
		VertexAttributeLayout(VertexAttributeType type, Math::ColorFormat format) :
			type(type),
			format(format)
		{
		}

		VertexAttributeType type;
		Math::ColorFormat format;
	};

	struct VertexLayout
	{
		List<VertexAttributeLayout> attributes;
		uint32 stride;
	};
}