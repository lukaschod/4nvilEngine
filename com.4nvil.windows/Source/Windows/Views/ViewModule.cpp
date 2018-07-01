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

#include <Core/Tools/Enum.hpp>
#include <Core/Graphics/IGraphicsModule.hpp>
#include <Core/Input/InputModule.hpp>
#include <Core/Input/MouseInputs.hpp>
#include <Core/Input/ViewInputs.hpp>
#include <Windows/Views/ViewModule.hpp>
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

Void ViewModule::SetupExecuteOrder(ModuleManager* moduleManager)
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

Void ViewModule::DestroyNativeWindow(const ExecutionContext& context, View* view)
{
    views.remove(view);
    DestroyWindow(view->windowHandle);
}

Void RecButtonInput(const ExecutionContext& context, InputModule* inputModule, const InputDevice* inputDevice, MouseButtonType type, Bool isDown)
{
    MouseButtonDesc desc;
    desc.isDown = isDown;
    desc.type = type;
    inputModule->RecInput(context, inputDevice, Enum::ToUnderlying(MouseInputType::Button), (UInt8*) &desc, sizeof(desc));
}

struct WndProcData
{
    ExecutionContext context;
    ViewModule* viewModule;
    const View* view;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto data = (WndProcData*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (data == nullptr)
        return DefWindowProc(hwnd, msg, wParam, lParam);

    return data->viewModule->HandleMessage(data->context, data->view, msg, wParam, lParam);
}

LRESULT CALLBACK ViewModule::HandleMessage(const ExecutionContext& context, const View* view, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_MOUSEMOVE:
    {
        auto x = GET_X_LPARAM(lParam);
        auto y = GET_Y_LPARAM(lParam);

        MousePositionDesc desc;
        desc.position = Vector2f((Float) x, (Float) y);
        inputModule->RecInput(context, inputDevice, Enum::ToUnderlying(MouseInputType::Move), (UInt8*) &desc, sizeof(desc));
        break;
    }

    case WM_SIZE:
    {
        ViewInputResizeDesc desc;
        desc.width = (UInt32) lParam;
        desc.height = (UInt32) (lParam << 32);
        inputModule->RecInput(context, view->viewInputDevice, Enum::ToUnderlying(ViewInputType::Resize), (UInt8*) &desc, sizeof(desc));
        inputModule->RecInput(context, view->viewInputDevice, Enum::ToUnderlying(ViewInputType::Render), (UInt8*) nullptr, 0);
        break;
    }

    case WM_LBUTTONUP: RecButtonInput(context, inputModule, inputDevice, MouseButtonType::Left, false); break;
    case WM_LBUTTONDOWN: RecButtonInput(context, inputModule, inputDevice, MouseButtonType::Left, true); break;
    case WM_RBUTTONUP: RecButtonInput(context, inputModule, inputDevice, MouseButtonType::Right, false); break;
    case WM_RBUTTONDOWN: RecButtonInput(context, inputModule, inputDevice, MouseButtonType::Right, true); break;
    case WM_MBUTTONUP: RecButtonInput(context, inputModule, inputDevice, MouseButtonType::Center, false); break;
    case WM_MBUTTONDOWN: RecButtonInput(context, inputModule, inputDevice, MouseButtonType::Center, true); break;

    case WM_CLOSE:
        RecDestroyIView(context, view);
        inputModule->RecInput(context, view->viewInputDevice, Enum::ToUnderlying(ViewInputType::Destroy), nullptr, 0);
        return -1; // Do not close window, we will close it in next frame

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(view->windowHandle, msg, wParam, lParam);
    }
    return 0;
}

// We must create it as static, because windows class is treated globaly in process
// For this reason there is not point for locality
static const Char* defaultWindowClassName = nullptr;
Bool ViewModule::RegisterDefaultWindowClass()
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

Void ViewModule::CreateNativeWindow(const ExecutionContext& context, View* view)
{
    // Create window class on demand
    if (defaultWindowClassName == nullptr)
        CHECK(RegisterDefaultWindowClass());

    // Create mouse inputs handling on demand
    if (inputDevice == nullptr)
    {
        InputDeviceDesc desc;
        desc.typeName = "Mouse";
        inputDevice = inputModule->RecCreateInputDevice(context, desc);
    }

    // TODO: Fix this normally to respect to actual current screen
    /*HDC screen = GetDC(0);
    Float dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
    Float dpiY = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSY));
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

    view->windowHandle = CreateWindowEx(
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

    CHECK(view->windowHandle != nullptr);

    // We have to add user data, because after UpdateWindow might some events be sent
    WndProcData wndProcData;
    wndProcData.viewModule = this;
    wndProcData.context = context;
    wndProcData.view = view;
    SetWindowLongPtr(view->windowHandle, GWLP_USERDATA, (LPARAM) &wndProcData);

    ShowWindow(view->windowHandle, true);
    UpdateWindow(view->windowHandle);
}

Void ViewModule::Execute(const ExecutionContext& context)
{
    base::Execute(context);

    WndProcData wndProcData;
    wndProcData.viewModule = this;
    wndProcData.context = context;

    MSG msg;
    for (auto view : views)
    {
        wndProcData.view = view;
        SetWindowLongPtr(view->windowHandle, GWLP_USERDATA, (LPARAM)&wndProcData);
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
SERIALIZE_METHOD_ARG1(ViewModule, DestroyIView, const IView*);
SERIALIZE_METHOD_ARG2(ViewModule, SetRect, const IView*, const Rectf&);
SERIALIZE_METHOD_ARG2(ViewModule, SetName, const IView*, const Char*);
SERIALIZE_METHOD_ARG2(ViewModule, SetParent, const IView*, const IView*);

Bool ViewModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateIView, View*, target);
        target->created = true;

        // Create input device, for tracking all events like size change...
        InputDeviceDesc desc;
        desc.typeName = "View";
        target->viewInputDevice = inputModule->RecCreateInputDevice(context, desc);

        CreateNativeWindow(context, target);
        ASSERT(target->windowHandle != nullptr);

        views.push_back(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(DestroyIView, View*, target);
        views.remove(target);
        DestroyNativeWindow(context, target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetRect, View*, target, const Rectf, rect);
        ASSERT(!target->created);
        target->width = (UInt32) rect.width;
        target->height = (UInt32) rect.height;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetName, View*, target, const Char*, name);
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
