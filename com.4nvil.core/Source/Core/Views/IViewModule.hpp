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

#include <Core/Tools/Common.hpp>
#include <Core/Tools/String.hpp>
#include <Core/Tools/Math/Rect.hpp>
#include <Core/Foundation/PipeModule.hpp>

namespace Core
{
    struct IView;
    struct InputDevice;
}

namespace Core
{
    enum class ViewType
    {
        Default,
        Parent,
        Child,
    };

    struct IView
    {
        IView()
            : type(ViewType::Parent)
            , width(2048)
            , height(1536)
            , name("Unamed")
            , parent(nullptr)
            , viewInputDevice(nullptr)
            , created(false)
        {
        }
        ViewType type;
        UInt32 width;
        UInt32 height;
        const Char* name;
        const IView* parent;
        const InputDevice* viewInputDevice;
        Bool created;
    };

    class IViewModule : public PipeModule
    {
    public:
        virtual const IView* AllocateView() = 0;
        virtual const List<const IView*>& GetViews() = 0;

    public:
        virtual Void RecCreateIView(const ExecutionContext& context, const IView* target) = 0;
        virtual Void RecSetRect(const ExecutionContext& context, const IView* target, const Math::Rectf& rect) = 0;
        virtual Void RecSetName(const ExecutionContext& context, const IView* target, const Char* name) = 0;
        virtual Void RecSetParent(const ExecutionContext& context, const IView* target, const IView* parent) = 0;
    };
}