#pragma once

#include <Core\Tools\Common.h>
#include <Core\Tools\String.h>
#include <Core\Tools\Math\Rect.h>
#include <Core\Foundation\PipeModule.h>

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

	struct ViewDesc
	{
		ViewDesc()
			: type(ViewType::Default)
			, width(2048)
			, height(1536)
			, parent(nullptr)
			, name("Unamed")
		{ }
		ViewType type;
		uint32 width;
		uint32 height;
		const IView* parent;
		const char* name;
	};

	struct IView
	{
		IView(const ViewDesc& desc)
			: type(desc.type)
			, width(desc.width)
			, height(desc.height)
			, name(desc.name)
			, parent(desc.parent)
			, viewInputDevice(nullptr)
		{
		}
		ViewType type;
		uint32 width;
		uint32 height;
		const char* name;
		const IView* parent;
		const InputDevice* viewInputDevice;
	};

	class IViewModule : public PipeModule
	{
	public:
		virtual const IView* RecCreateIView(const ExecutionContext& context, const IView* view = nullptr) = 0;
		virtual const IView* RecCreateIView(const ExecutionContext& context, const ViewDesc& desc, const IView* view = nullptr) = 0;
		virtual const List<const IView*>& GetViews() = 0;
	};
}