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

#pragma once

#include <Core/Tools/Windows/Common.hpp>
#include <Core/Rendering/ImageModule.hpp>
#include <Core/Views/IViewModule.hpp>

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
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        virtual Void Execute(const ExecutionContext& context) override;
        virtual const IView* AllocateView() override;

        // Returns all views that are currently allocated
        virtual const List<const IView*>& GetViews() override;

        // Handle specific windows message
        LRESULT CALLBACK HandleMessage(const ExecutionContext& context, const View* view, UINT msg, WPARAM wParam, LPARAM lParam);

    public:
        virtual Void RecCreateView(const ExecutionContext& context, const IView* target) override;

        virtual Void RecDestroyView(const ExecutionContext& context, const IView* target) override;

        // Set view position and size
        // Can't be called after creation of view
        virtual Void RecSetRect(const ExecutionContext& context, const IView* target, const Math::Rectf& rect) override;

        // Set name to view
        // Can't be called after creation of view
        virtual Void RecSetName(const ExecutionContext& context, const IView* target, const Char* name) override;

        // Set parent to view
        // Can't be called after creation of view
        virtual Void RecSetParent(const ExecutionContext& context, const IView* target, const IView* parent) override;

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Bool RegisterDefaultWindowClass();
        Void CreateNativeWindow(const ExecutionContext& context, View* view);
        Void DestroyNativeWindow(const ExecutionContext& context, View* view);

    private:
        Graphics::IGraphicsModule* graphicsModule;
        InputModule* inputModule;
        const InputDevice* inputDevice;
        ImageModule* imageModule;
        List<View*> views;
        HINSTANCE instanceHandle;
    };
}