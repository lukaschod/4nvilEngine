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
	kTextureUsageFlagNone = 0,
	kTextureUsageFlagRender = 1 << 0,
	kTextureUsageFlagShader = 1 << 1,
};

IMPLEMENT_ENUM_FLAG(TextureUsageFlags);

struct ITexture
{
	ITexture() {}
	ITexture(uint32_t width, uint32_t height) : 
		width(width), 
		height(height), 
		usage(kTextureUsageFlagRender | kTextureUsageFlagShader)
	{
	}
	uint32_t width;
	uint32_t height;
	TextureUsageFlags usage;
};