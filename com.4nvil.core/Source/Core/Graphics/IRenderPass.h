#pragma once

#include <Core\Tools\Common.h>
#include <Core\Tools\Math\Color.h>
#include <Core\Tools\Math\Rect.h>
#include <Core\Tools\Collections\List.h>
#include <Core\Graphics\ITexture.h>

namespace Core::Graphics
{
#define COLOR_ATTACHMENT_MAX_COUNT 4 // TODO: Remove this!!!!!!!!!!!

	enum class StoreAction
	{
		Store,
	};

	enum class LoadAction
	{
		Load,
		Clear,
		DontCare,
	};

	struct ColorAttachment
	{
		ColorAttachment() : ColorAttachment(nullptr, StoreAction::Store, LoadAction::Load) {}
		ColorAttachment(const ITexture* texture) : ColorAttachment(texture, StoreAction::Store, LoadAction::Load) {}
		ColorAttachment(const ITexture* texture, StoreAction storeAction, LoadAction loadAction) :
			texture(texture), storeAction(storeAction), loadAction(loadAction), clearColor(Math::Colorf(0, 0.2f, 0.4f, 0))
		{
		}

		const ITexture* texture;
		StoreAction storeAction;
		LoadAction loadAction;
		Math::Colorf clearColor;
	};

	struct DepthAttachment
	{
		DepthAttachment(const ITexture* texture, StoreAction storeAction, LoadAction loadAction) :
			texture(texture), storeAction(storeAction), loadAction(loadAction), clearDepth(0)
		{
		}
		DepthAttachment() : DepthAttachment(nullptr, StoreAction::Store, LoadAction::Load) {}

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
		Viewport(const Math::Rectf& rect) :
			rect(rect),
			nearClipPlane(0.1f),
			farClipPlane(10000)
		{
		}
		Math::Rectf rect;
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
}