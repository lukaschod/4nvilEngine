#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Tools\Collections\List.hpp>
#include <Core\Tools\Math\Color.hpp>

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