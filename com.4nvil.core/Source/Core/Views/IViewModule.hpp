#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Tools\String.hpp>
#include <Core\Tools\Math\Rect.hpp>
#include <Core\Foundation\PipeModule.hpp>

namespace Core
{
	struct IView;
	struct InputDevice;
}

namespace Core
{
	enum class ViewType
	{
		Default,
		Parent,
		Child,
	};

	struct IView
	{
		IView()
			: type(ViewType::Parent)
			, width(2048)
			, height(1536)
			, name("Unamed")
			, parent(nullptr)
			, viewInputDevice(nullptr)
			, created(false)
		{
		}
		ViewType type;
		uint32 width;
		uint32 height;
		const char* name;
		const IView* parent;
		const InputDevice* viewInputDevice;
		bool created;
	};

	class IViewModule : public PipeModule
	{
	public:
		virtual const IView* AllocateView() = 0;
		virtual const List<const IView*>& GetViews() = 0;

	public:
		virtual void RecCreateIView(const ExecutionContext& context, const IView* target) = 0;
		virtual void RecSetRect(const ExecutionContext& context, const IView* target, const Math::Rectf& rect) = 0;
		virtual void RecSetName(const ExecutionContext& context, const IView* target, const char* name) = 0;
		virtual void RecSetParent(const ExecutionContext& context, const IView* target, const IView* parent) = 0;
	};
}