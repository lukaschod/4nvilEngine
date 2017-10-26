#pragma once

#include <Common\EngineCommon.h>

struct FilterOptions
{

};

struct IFilter
{
	IFilter(const FilterOptions& options)
	{

	}
};

enum TextureUsageFlags
{
	TextureUsageFlagNone = 0,
	TextureUsageFlagRender = 1 << 0,
	TextureUsageFlagShader = 1 << 1,
};

IMPLEMENT_ENUM_FLAG(TextureUsageFlags);

struct ITexture
{
	ITexture() {}
	ITexture(uint32_t width, uint32_t height) : 
		width(width), 
		height(height), 
		usage(TextureUsageFlagRender | TextureUsageFlagShader)
	{
	}
	uint32_t width;
	uint32_t height;
	TextureUsageFlags usage;
};