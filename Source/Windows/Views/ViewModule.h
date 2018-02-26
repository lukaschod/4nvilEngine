#pragma once

#include <Tools\Windows\Common.h>
#include <Rendering\ImageModule.h>
#include <Views\IViewModule.h>

using namespace Core;

namespace Core
{
	class InputModule;
}

namespace Core::Graphics
{
	class IGraphicsModule;
}

namespace Windows
{
	struct View : public IView
	{
		View(const Image* renderTarget)
			: IView(renderTarget)
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
		const IView* AllocateView() const;

	public:
		virtual const IView* RecCreateIView(const ExecutionContext& context, const IView* view = nullptr) override;
		virtual const List<const IView*>& GetViews() override;
		void CloseWindow(HWND windowHandle);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		bool RegisterDefaultWindowClass();
		HWND TryCreateWindow(const char* name, uint32 width, uint32 height);
		View* TryFindView(HWND windowHandle);

	private:
		Graphics::IGraphicsModule* graphicsModule;
		InputModule* inputModule;
		ImageModule* imageModule;
		List<View*> views;
		const char* defaultWindowClassName;
		HINSTANCE instanceHandle;
	};
}