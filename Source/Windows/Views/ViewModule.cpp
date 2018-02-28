#include <Tools\Enum.h>
#include <Graphics\IGraphicsModule.h>
#include <Input\InputModule.h>
#include <Input\MouseInputs.h>
#include <Windows\Views\ViewModule.h>
#include <windowsx.h>

using namespace Core;
using namespace Core::Graphics;
using namespace Windows;

ViewModule::ViewModule(HINSTANCE instanceHandle)
	: instanceHandle(instanceHandle)
	, inputDevice(nullptr)
{
}

void ViewModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
	imageModule = ExecuteBefore<ImageModule>(moduleManager);
	inputModule = ExecuteBefore<InputModule>(moduleManager);
}

const List<const IView*>& ViewModule::GetViews()
{
	auto data = (List<const IView*>*)&views;
	return *data;
}

void ViewModule::CloseWindow(HWND windowHandle)
{
	auto view = TryFindView(windowHandle);
	ASSERT(view != nullptr);
	views.remove(view);
	DestroyWindow(windowHandle);
}

struct WndProcData
{
	ExecutionContext context;
	ViewModule* viewModule;
	InputModule* inputModule;
	const InputDevice* inputDevice;
};

static WndProcData wndProcData;
static void RecButtonInput(MouseButtonType type, bool isDown)
{
	MouseButtonDesc desc;
	desc.isDown = isDown;
	desc.type = type;
	wndProcData.inputModule->RecInput(wndProcData.context, wndProcData.inputDevice, Enum::ToUnderlying(MouseInputType::Button), (uint8*) &desc, sizeof(MouseButtonDesc));
	TRACE("%d %d", type, isDown);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
	{
		auto xPos = GET_X_LPARAM(lParam);
		auto yPos = GET_Y_LPARAM(lParam);
		MousePositionDesc desc;
		desc.position = Math::Vector2f((float) xPos, (float) yPos);
		wndProcData.inputModule->RecInput(wndProcData.context, wndProcData.inputDevice, Enum::ToUnderlying(MouseInputType::Move), (uint8*) &desc, sizeof(MousePositionDesc));
		break;
	}

	case WM_LBUTTONUP: RecButtonInput(MouseButtonType::Left, false); break;
	case WM_LBUTTONDOWN: RecButtonInput(MouseButtonType::Left, true); break;
	case WM_RBUTTONUP: RecButtonInput(MouseButtonType::Right, false); break;
	case WM_RBUTTONDOWN: RecButtonInput(MouseButtonType::Right, true); break;
	case WM_MBUTTONUP: RecButtonInput(MouseButtonType::Center, false); break;
	case WM_MBUTTONDOWN: RecButtonInput(MouseButtonType::Center, true); break;

	case WM_CLOSE:
		ASSERT(wndProcData.viewModule != nullptr);
		wndProcData.viewModule->CloseWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

bool ViewModule::RegisterDefaultWindowClass()
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

HWND ViewModule::TryCreateWindow(const char* name, uint32 width, uint32 height)
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

View* ViewModule::TryFindView(HWND windowHandle)
{
	for (auto view : views)
	{
		if (view->windowHandle == windowHandle)
			return view;
	}
	return nullptr;
}

void ViewModule::Execute(const ExecutionContext& context)
{
	PipeModule::Execute(context);

	if (inputDevice == nullptr)
	{
		InputDeviceDesc desc;
		desc.typeName = "Mouse";
		desc.vendorName = "Unknown";
		inputDevice = inputModule->RecCreateInputDevice(context, desc);
	}

	// TODO: Lets figure out if we can pass the object to callback somehow
	wndProcData.viewModule = this;
	wndProcData.context = context;
	wndProcData.inputModule = inputModule;
	wndProcData.inputDevice = inputDevice;

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

const IView* ViewModule::AllocateView() const
{
	// TODO: Make it passible
	auto width = 2048;
	auto height = 1536;

	auto renderTarget = imageModule->AllocateImage(width, height);
	auto view = new View(renderTarget);
	view->width = width;
	view->height = height;
	view->swapChain = graphicsModule->AllocateSwapChain(view);
	return view;
}

DECLARE_COMMAND_CODE(CreateIView);
const IView* ViewModule::RecCreateIView(const ExecutionContext& context, const IView* view)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = view == nullptr ? AllocateView() : view;
	stream.Write(TO_COMMAND_CODE(CreateIView));
	stream.Write(target);
	stream.Align();
	buffer->commandCount++;
	return target;
}

bool ViewModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateIView, View*, target);
		imageModule->RecCreateImage(context, 0, 0, target->renderTarget);
		graphicsModule->RecCreateISwapChain(context, nullptr, target->swapChain);
		target->windowHandle = TryCreateWindow("Unnamed", target->width, target->height);
		ASSERT(target->windowHandle != nullptr);
		views.push_back(target);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
