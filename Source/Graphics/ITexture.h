#pragma once

#include <Tools\Common.h>

namespace Core::Graphics
{
	enum class TextureUsageFlags
	{
		None = 0,
		Render = 1 << 0,
		Shader = 1 << 1,
	};
	IMPLEMENT_ENUM_FLAG(TextureUsageFlags);

	struct ITexture
	{
		ITexture() {}
		ITexture(uint32_t width, uint32_t height) :
			width(width),
			height(height),
			usage(TextureUsageFlags::Render | TextureUsageFlags::Shader)
		{
		}
		uint32_t width;
		uint32_t height;
		TextureUsageFlags usage;
	};
}