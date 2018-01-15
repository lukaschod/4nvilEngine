#include <Windows\Views\WinViewModule.h>
#include <Windows\Common.h>

WinViewModule::WinViewModule(HINSTANCE instanceHandle)
	: instanceHandle(instanceHandle)
{
}

void WinViewModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	PipeModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
	imageModule = ExecuteBefore<ImageModule>(moduleManager);
}

const List<const IView*>& WinViewModule::GetViews()
{
	auto data = (List<const IView*>*)&views;
	return *data;
}

void WinViewModule::CloseWindow(HWND windowHandle)
{
	auto view = TryFindView(windowHandle);
	ASSERT(view != nullptr);
	views.remove(view);
	DestroyWindow(windowHandle);
}

static WinViewModule* viewModule = nullptr;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ASSERT(viewModule != nullptr);
	switch (msg)
	{
	case WM_CLOSE:
		viewModule->CloseWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

bool WinViewModule::RegisterDefaultWindowClass()
{
	ASSERT(instanceHandle != nullptr);
	ASSERT(defaultWindowClassName == nullptr);
	defaultWindowClassName = "DefaultWindowClass";
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instanceHandle;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = defaultWindowClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	return RegisterClassEx(&wc) != 0;
}

HWND WinViewModule::TryCreateWindow(const char* name, uint32_t width, uint32_t height)
{
	if (defaultWindowClassName == nullptr && !RegisterDefaultWindowClass())
		return nullptr;

	// TODO: Fix this normally to respect to actual current screen
	/*HDC screen = GetDC(0);
	float dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
	float dpiY = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSY));
	ReleaseDC(0, screen);*/

	ASSERT(defaultWindowClassName != nullptr);
	HWND hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		defaultWindowClassName,
		name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		NULL, NULL, instanceHandle, NULL);

	auto dpi = GetDpiForWindow(hwnd);

	if (hwnd == nullptr)
		return nullptr;

	ShowWindow(hwnd, true);
	UpdateWindow(hwnd);

	return hwnd;
}

WinView* WinViewModule::TryFindView(HWND windowHandle)
{
	for (auto view : views)
	{
		if (view->windowHandle == windowHandle)
			return view;
	}
	return nullptr;
}

void WinViewModule::Execute(const ExecutionContext& context)
{
	PipeModule::Execute(context);

	viewModule = this; // TODO: Lets figure out if we can pass the object to callback somehow
	MSG msg;
	for (auto view : views)
	{
		while (PeekMessage(&msg, view->windowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

const IView* WinViewModule::AllocateView() const
{
	// TODO: Make it passible
	auto width = 2048;
	auto height = 1536;

	auto renderTarget = imageModule->AllocateImage(width, height);
	auto view = new WinView(renderTarget);
	view->width = width;
	view->height = height;
	view->swapChain = graphicsModule->AllocateSwapChain(view);
	return view;
}

DECLARE_COMMAND_CODE(CreateIView);
const IView* WinViewModule::RecCreateIView(const ExecutionContext& context, const IView* view)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = view == nullptr ? AllocateView() : view;
	stream.Write(CommandCodeCreateIView);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}

bool WinViewModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateIView, WinView*, target);
		imageModule->RecCreateImage(context, 0, 0, target->renderTarget);
		graphicsModule->RecCreateISwapChain(context, nullptr, target->swapChain);
		target->windowHandle = TryCreateWindow("Unnamed", target->width, target->height);
		ASSERT(target->windowHandle != nullptr);
		views.push_back(target);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
