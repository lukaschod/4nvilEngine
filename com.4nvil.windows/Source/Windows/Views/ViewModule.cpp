/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <Core\Tools\Enum.hpp>
#include <Core\Graphics\IGraphicsModule.hpp>
#include <Core\Input\InputModule.hpp>
#include <Core\Input\MouseInputs.hpp>
#include <Core\Input\ViewInputs.hpp>
#include <Windows\Views\ViewModule.hpp>
#include <windowsx.h>

using namespace Core;
using namespace Core::Math;
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
    const InputDevice* mouseInputDevice;
    const View* view;
};
static WndProcData wndProcData;

static void RecButtonInput(MouseButtonType type, bool isDown)
{
    MouseButtonDesc desc;
    desc.isDown = isDown;
    desc.type = type;
    wndProcData.inputModule->RecInput(wndProcData.context, wndProcData.mouseInputDevice, Enum::ToUnderlying(MouseInputType::Button), (uint8*) &desc, sizeof(desc));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    TRACE("%x %d %d", msg, wParam, lParam);
    switch (msg)
    {
    case WM_MOUSEMOVE:
    {
        auto xPos = GET_X_LPARAM(lParam);
        auto yPos = GET_Y_LPARAM(lParam);

        MousePositionDesc desc;
        desc.position = Vector2f((float) xPos, (float) yPos);
        wndProcData.inputModule->RecInput(wndProcData.context, wndProcData.mouseInputDevice, Enum::ToUnderlying(MouseInputType::Move), (uint8*) &desc, sizeof(desc));
        break;
    }

    case WM_SIZE:
    {
        if (wndProcData.inputModule == nullptr)
            return 0;

        ViewInputResizeDesc desc;
        desc.width = (uint32) lParam;
        desc.height = (uint32) (lParam << 32);
        wndProcData.inputModule->RecInput(wndProcData.context, wndProcData.view->viewInputDevice, Enum::ToUnderlying(ViewInputType::Resize), (uint8*) &desc, sizeof(desc));
        wndProcData.inputModule->RecInput(wndProcData.context, wndProcData.view->viewInputDevice, Enum::ToUnderlying(ViewInputType::Render), (uint8*) nullptr, 0);
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

HWND ViewModule::TryCreateWindow(const IView* view)
{
    if (defaultWindowClassName == nullptr && !RegisterDefaultWindowClass())
        return nullptr;

    // TODO: Fix this normally to respect to actual current screen
    /*HDC screen = GetDC(0);
    float dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
    float dpiY = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSY));
    ReleaseDC(0, screen);*/

    DWORD windowStyle = WS_OVERLAPPEDWINDOW;
    DWORD extendedStyle = WS_EX_CLIENTEDGE;
    HWND parentHWND = NULL;

    switch (view->type)
    {
    case ViewType::Default:
        break;
    case ViewType::Parent:
    {
        windowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
        parentHWND = NULL;
        break;
    }

    case ViewType::Child:
    {
        windowStyle = WS_CHILD | WS_CLIPCHILDREN;
        extendedStyle = WS_EX_TOOLWINDOW;
        auto parent = (const View*) view->parent;
        ASSERT(parent != nullptr);
        parentHWND = parent->windowHandle;
        break;
    }

    default:
        ERROR("Unknow window type");
    }

    ASSERT(defaultWindowClassName != nullptr);
    HWND hwnd = CreateWindowEx(
        extendedStyle,
        defaultWindowClassName,
        view->name,
        windowStyle,
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        view->width, 
        view->height,
        parentHWND,
        NULL, 
        instanceHandle, 
        NULL);

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
    base::Execute(context);

    if (inputDevice == nullptr)
    {
        InputDeviceDesc desc;
        desc.typeName = "Mouse";
        inputDevice = inputModule->RecCreateInputDevice(context, desc);
    }

    // TODO: Lets figure out if we can pass the object to callback somehow
    wndProcData.viewModule = this;
    wndProcData.context = context;
    wndProcData.inputModule = inputModule;
    wndProcData.mouseInputDevice = inputDevice;

    MSG msg;
    for (auto view : views)
    {
        wndProcData.view = view;
        while (PeekMessage(&msg, view->windowHandle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

const IView* ViewModule::AllocateView()
{
    return new View();
}

SERIALIZE_METHOD_ARG1(ViewModule, CreateIView, const IView*);
SERIALIZE_METHOD_ARG2(ViewModule, SetRect, const IView*, const Rectf&);
SERIALIZE_METHOD_ARG2(ViewModule, SetName, const IView*, const char*);
SERIALIZE_METHOD_ARG2(ViewModule, SetParent, const IView*, const IView*);

bool ViewModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateIView, View*, target);
        target->created = true;
        target->windowHandle = TryCreateWindow(target);
        ASSERT(target->windowHandle != nullptr);

        // Create input device, for tracking all events like size change...
        InputDeviceDesc desc;
        desc.typeName = "View";
        target->viewInputDevice = inputModule->RecCreateInputDevice(context, desc);

        views.push_back(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetRect, View*, target, const Rectf, rect);
        ASSERT(!target->created);
        target->width = (uint32) rect.width;
        target->height = (uint32) rect.height;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetName, View*, target, const char*, name);
        ASSERT(!target->created);
        target->name = name;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetParent, View*, target, const IView*, parent);
        ASSERT(!target->created);
        target->parent = parent;
        target->type = ViewType::Child;
        DESERIALIZE_METHOD_END;
    }
    return false;
}
