#pragma once

#include <Core\Tools\Windows\Common.hpp>
#include <Core\Rendering\ImageModule.hpp>
#include <Core\Views\IViewModule.hpp>

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
		View()
			: windowHandle(nullptr)
		{}
		HWND windowHandle;
	};

	class ViewModule : public IViewModule
	{
	public:
		BASE_IS(IViewModule);

		ViewModule(HINSTANCE instanceHandle);
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context) override;
		virtual const IView* AllocateView() override;

		// Returns all views that are currently allocated
		virtual const List<const IView*>& GetViews() override;

		// Close window by window handle
		void CloseWindow(HWND windowHandle);

	public:
		virtual void RecCreateIView(const ExecutionContext& context, const IView* target) override;

		// Set view position and size
		// Can't be called after creation of view
		virtual void RecSetRect(const ExecutionContext& context, const IView* target, const Math::Rectf& rect) override;

		// Set name to view
		// Can't be called after creation of view
		virtual void RecSetName(const ExecutionContext& context, const IView* target, const char* name) override;

		// Set parent to view
		// Can't be called after creation of view
		virtual void RecSetParent(const ExecutionContext& context, const IView* target, const IView* parent) override;

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