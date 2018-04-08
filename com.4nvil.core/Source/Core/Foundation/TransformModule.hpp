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

#include <queue>
#include <Core/Tools/Common.hpp>
#include <Core/Tools/Flags.hpp>
#include <Core/Tools/Math/Matrix.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Foundation/UnitModule.hpp>

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
        Flags<TransformStateFlags> flags;
        bool created;
    };

    class TransformModule : public ComponentModule
    {
    public:
        BASE_IS(ComponentModule);

        virtual void Execute(const ExecutionContext& context) override;
        virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Transform* AllocateTransform();

    public:
        void RecCreateTransform(const ExecutionContext& context, const Transform* target);
        virtual void RecDestroy(const ExecutionContext& context, const Component* unit) override;
        void RecSetParent(const ExecutionContext& context, const Transform* target, const Transform* parent);
        void RecSetPosition(const ExecutionContext& context, const Transform* target, const Math::Vector3f& position);
        void RecAddPosition(const ExecutionContext& context, const Transform* target, const Math::Vector3f& position);
        void RecSetRotation(const ExecutionContext& context, const Transform* target, const Math::Vector3f& rotation);

        // Recalculate worldToView matrix for transform
        void RecCalculateWorldToView(const ExecutionContext& context, const Transform* target);

    protected:
        virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        void SetParent(const ExecutionContext& context, Transform* target, Transform* parent);

    private:
        MemoryModule* memoryModule;
        Transform* root;
        std::queue<Transform*> transformsToCalculate;
        std::vector<Transform*> worldToViewToCalculate;
    };
}