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
#include <Core/Tools/Enum.hpp>
#include <Core/Tools/Math/Matrix.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Foundation/UnitModule.hpp>
#include <queue>

namespace Core
{
    class MemoryModule;
}

namespace Core
{
    enum class TransformStateFlags
    {
        None = 0,
        LocalObjectToWorldChanged = 1 << 0,
        LocalObjectToWorldUnsetNextFrame = 1 << 1,
    };
    IMPLEMENT_ENUM_FLAG(TransformStateFlags);

    struct Transform : public Component
    {
        Transform(ComponentModule* module)
            : Component(module)
            , parent(nullptr)
            , localPosition(0, 0, 0)
            , localRotation(0, 0, 0, 1)
            , localScale(1, 1, 1)
            , created(false)
        {
        }

        List<Transform*> childs;
        Transform* parent;
        Math::Matrix4x4f objectToWorld;
        Math::Matrix4x4f worldToView;
        Math::Matrix4x4f localObjectToWorld;
        Math::Vector3f localPosition;
        Math::Quaternionf localRotation;
        Math::Vector3f localScale;
        Math::Vector3f position;
        TransformStateFlags flags;
        Bool created;
    };

    class TransformModule : public ComponentModule
    {
    public:
        BASE_IS(ComponentModule);

        virtual Void Execute(const ExecutionContext& context) override;
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Transform* AllocateTransform();

    public:
        Void RecCreateTransform(const ExecutionContext& context, const Transform* target);
        virtual Void RecDestroy(const ExecutionContext& context, const Component* unit) override;
        Void RecSetParent(const ExecutionContext& context, const Transform* target, const Transform* parent);
        Void RecSetPosition(const ExecutionContext& context, const Transform* target, const Math::Vector3f& position);
        Void RecAddPosition(const ExecutionContext& context, const Transform* target, const Math::Vector3f& position);
        Void RecSetRotation(const ExecutionContext& context, const Transform* target, const Math::Vector3f& rotation);

        // Recalculate worldToView matrix for transform
        Void RecCalculateWorldToView(const ExecutionContext& context, const Transform* target);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Void SetParent(const ExecutionContext& context, Transform* target, Transform* parent);

    private:
        MemoryModule* memoryModule;
        Transform* root;
        std::queue<Transform*> transformsToCalculate;
        std::vector<Transform*> worldToViewToCalculate;
    };
}