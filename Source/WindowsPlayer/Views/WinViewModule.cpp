#include <WindowsPlayer\Views\WinViewModule.h>

WinViewModule::WinViewModule(uint32_t bufferCount, uint32_t bufferIndexStep, HINSTANCE instanceHandle) :
	IViewModule(bufferCount, bufferIndexStep),
	instanceHandle(instanceHandle)
{
}

void WinViewModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
	imageModule = ExecuteBefore<ImageModule>(moduleManager);
}

const List<const IView*>& WinViewModule::GetViews()
{
	auto data = (List<const IView*>*)&views;
	return *data;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
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

	ASSERT(defaultWindowClassName != nullptr);
	HWND hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		defaultWindowClassName,
		name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		NULL, NULL, instanceHandle, NULL);

	if (hwnd == nullptr)
		return nullptr;

	ShowWindow(hwnd, true);
	UpdateWindow(hwnd);

	return hwnd;
}

void WinViewModule::Execute(const ExecutionContext& context)
{
	CmdModule::Execute(context);

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

DECLARE_COMMAND_CODE(CreateIView);
const IView* WinViewModule::RecCreateIView(const ExecutionContext& context)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto width = 2096;
	auto height = 1280;
	auto renderTarget = imageModule->RecCreateImage(context, width, height);
	auto target = new WinView(renderTarget);
	target->width = width;
	target->height = height;
	auto swapChain = graphicsModule->RecCreateISwapChain(context, target);
	target->swapChain = swapChain;
	stream.Write(kCommandCodeCreateIView);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}

bool WinViewModule::ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateIView, WinView*, target);
		target->windowHandle = TryCreateWindow("Unnamed", target->width, target->height);
		ASSERT(target->windowHandle != nullptr);
		views.push_back(target);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
