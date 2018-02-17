#pragma once

#include <Tools\Common.h>
#include <Graphics\IGraphicsModule.h>
#include <Views\IViewModule.h>
#include <windows.h>

struct WinView : public IView
{
	WinView(const Image* renderTarget)
		: IView(renderTarget)
	{}
	HWND windowHandle;
};

class WinViewModule : public IViewModule
{
public:
	WinViewModule(HINSTANCE instanceHandle);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;
	const IView* AllocateView() const;

public:
	virtual const IView* RecCreateIView(const ExecutionContext& context, const IView* view = nullptr) override;
	virtual const List<const IView*>& GetViews() override;
	void CloseWindow(HWND windowHandle);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, CommandCode commandCode) override;

private:
	bool RegisterDefaultWindowClass();
	HWND TryCreateWindow(const char* name, uint32_t width, uint32_t height);
	WinView* TryFindView(HWND windowHandle);

private:
	IGraphicsModule* graphicsModule;
	ImageModule* imageModule;
	List<WinView*> views;
	const char* defaultWindowClassName;
	HINSTANCE instanceHandle;
};