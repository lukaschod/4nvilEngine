#pragma once

#include <Common\EngineCommon.h>
#include <Common\Collections\List.h>
#include <Math\Color.h>

enum VertexAttributeType
{
	VertexAttributeTypePosition,
	VertexAttributeTypeTexCoord0,
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