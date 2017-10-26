#pragma once

#include <Common\EngineCommon.h>
#include <Graphics\ITexture.h>
#include <Common\Collections\List.h>
#include <Math\Color.h>
#include <Math\Rect.h>

#define COLOR_ATTACHMENT_MAX_COUNT 4

enum StoreAction
{
	StoreActionStore,
};

enum LoadAction
{
	LoadActionLoad,
	LoadActionClear,
	LoadActionDontCare,
};

struct ColorAttachment
{
	ColorAttachment() : ColorAttachment(nullptr, StoreActionStore, LoadActionLoad) {}
	ColorAttachment(const ITexture* texture) : ColorAttachment(texture, StoreActionStore, LoadActionLoad) {}
	ColorAttachment(const ITexture* texture, StoreAction storeAction, LoadAction loadAction) :
		texture(texture), storeAction(storeAction), loadAction(loadAction), clearColor(Colorf(0, 0.2f, 0.4f, 0))
	{
	}

	const ITexture* texture;
	StoreAction storeAction;
	LoadAction loadAction;
	Colorf clearColor;
};

struct DepthAttachment
{
	DepthAttachment(const ITexture* texture, StoreAction storeAction, LoadAction loadAction) :
		texture(texture), storeAction(storeAction), loadAction(loadAction), clearDepth(0)
	{
	}
	DepthAttachment() : DepthAttachment(nullptr, StoreActionStore, LoadActionLoad) {}

	const ITexture* texture;
	StoreAction storeAction;
	LoadAction loadAction;
	float clearDepth;
};

struct Viewport
{
	Viewport() :
		rect(0, 0, 1, 1),
		nearClipPlane(0.1f),
		farClipPlane(10000)
	{
	}
	Viewport(const Rectf& rect) :
		rect(rect),
		nearClipPlane(0.1f),
		farClipPlane(10000)
	{
	}
	Rectf rect;
	float nearClipPlane;
	float farClipPlane;
};

struct IRenderPass
{
	IRenderPass()
	{
		memset(colors, 0, sizeof(ColorAttachment));
	}

	inline float GetAspect() const
	{
		auto texture = colors[0].texture;
		ASSERT(texture != nullptr);
		return (float) texture->width / texture->height;
	}

	ColorAttachment colors[COLOR_ATTACHMENT_MAX_COUNT];
	DepthAttachment depth;
	Viewport viewport;
};