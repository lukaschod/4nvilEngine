#pragma once

#include <Tools\Common.h>
#include <Tools\Collections\List.h>
#include <Tools\Math\Color.h>

enum class VertexAttributeType
{
	Position,
	TexCoord0,
};

struct VertexAttributeLayout
{
	VertexAttributeLayout() {}
	VertexAttributeLayout(VertexAttributeType type, ColorFormat format) :
		type(type),
		format(format)
	{
	}

	VertexAttributeType type;
	ColorFormat format;
};

struct VertexLayout
{
	List<VertexAttributeLayout> attributes;
	uint32_t stride;
};