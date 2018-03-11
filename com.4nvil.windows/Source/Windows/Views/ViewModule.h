#pragma once

#include <Core\Tools\Windows\Common.h>
#include <Core\Rendering\ImageModule.h>
#include <Core\Views\IViewModule.h>

using namespace Core;

namespace Core
{
	struct InputDevice;
	class InputModule;

	namespace Graphics
	{
		class IGraphicsModule;
	}
}

namespace Windows
{
	struct View : public IView
	{
		View(const ViewDesc& desc) : IView(desc)
		{
		}
		HWND windowHandle;
	};

	class ViewModule : public IViewModule
	{
	public:
		ViewModule(HINSTANCE instanceHandle);
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context) override;
		const IView* AllocateView(const ViewDesc& desc) const;

	public:
		virtual const IView* RecCreateIView(const ExecutionContext& context, const IView* view = nullptr) override;
		virtual const IView* RecCreateIView(const ExecutionContext& context, const ViewDesc& desc, const IView* view = nullptr) override;
		virtual const List<const IView*>& GetViews() override;
		void CloseWindow(HWND windowHandle);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		bool RegisterDefaultWindowClass();
		HWND TryCreateWindow(const IView* view);
		View* TryFindView(HWND windowHandle);

	private:
		Graphics::IGraphicsModule* graphicsModule;
		InputModule* inputModule;
		const InputDevice* inputDevice;
		ImageModule* imageModule;
		List<View*> views;
		const char* defaultWindowClassName;
		HINSTANCE instanceHandle;
	};
}