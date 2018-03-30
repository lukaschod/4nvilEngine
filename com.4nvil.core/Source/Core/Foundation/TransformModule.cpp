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

#include <Core\Tools\Math\Math.hpp>
#include <Core\Tools\Collections\FixedBlockHeap.hpp>
#include <Core\Foundation\TransformModule.hpp>
#include <Core\Foundation\MemoryModule.hpp>

using namespace Core;
using namespace Core::Math;

static const char* memoryLabelTransform = "Core::Transform";

TransformModule::TransformModule() 
{
    root = new Transform(this);
    root->objectToWorld = Matrix4x4f::TRS(root->localPosition, root->localRotation, root->localScale);
}

void TransformModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
    memoryModule->SetAllocator(memoryLabelTransform, new FixedBlockHeap(sizeof(Transform)));
    unitModule = ExecuteAfter<UnitModule>(moduleManager);
}

void TransformModule::Execute(const ExecutionContext& context)
{
    MARK_FUNCTION;
    base::Execute(context);

    // Do BFS to re-calculate new transformations
    // TODO: Find performance BFS vs DFS, but my guess BFS better because cache might be used much more better
    for (auto child : root->childs)
        transformsToCalculate.push(child);
    while (!transformsToCalculate.empty())
    {
        // Get next transform to calculate
        auto next = transformsToCalculate.front();
        transformsToCalculate.pop();

        auto parent = next->parent;
        ASSERT(parent != nullptr);

        // Check if we need to unmark changed state
        if (next->flags.Contains(TransformStateFlags::LocalObjectToWorldUnsetNextFrame))
        {
            next->flags.Remove(TransformStateFlags::LocalObjectToWorldChanged);
            next->flags.Remove(TransformStateFlags::LocalObjectToWorldUnsetNextFrame);
        }

        // Check if we need update
        if (next->flags.Contains(TransformStateFlags::LocalObjectToWorldChanged))
        {
            next->localObjectToWorld = Matrix4x4f::TRS(next->localPosition, next->localRotation, next->localScale);
            next->flags.Add(TransformStateFlags::LocalObjectToWorldUnsetNextFrame);
        }

        // If nor local transformation changed nor the parent one, we can skip the combination of them
        if (next->flags.Contains(TransformStateFlags::LocalObjectToWorldChanged) || parent->flags.Contains(TransformStateFlags::LocalObjectToWorldChanged))
        {
            next->objectToWorld = next->localObjectToWorld;
            next->objectToWorld.Multiply(parent->objectToWorld);
            ASSERT(next->objectToWorld.IsValid());
            next->position = next->objectToWorld.GetPosition().xyz();
        }

        // Add transform childs
        for (auto child : next->childs)
            transformsToCalculate.push(child);
    }
}

const Transform* TransformModule::AllocateTransform()
{
    return memoryModule->New<Transform>(memoryLabelTransform, this);
}

SERIALIZE_METHOD_ARG1(TransformModule, CreateTransform, const Transform*);
SERIALIZE_METHOD_ARG1(TransformModule, Destroy, const Component*);
SERIALIZE_METHOD_ARG2(TransformModule, SetParent, const Transform*, const Transform*);
SERIALIZE_METHOD_ARG2(TransformModule, SetPosition, const Transform*, const Vector3f&);
SERIALIZE_METHOD_ARG2(TransformModule, AddPosition, const Transform*, const Vector3f&);
SERIALIZE_METHOD_ARG2(TransformModule, SetRotation, const Transform*, const Vector3f&);
SERIALIZE_METHOD_ARG1(TransformModule, CalculateWorldToView, const Transform*);

bool TransformModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateTransform, Transform*, target);
        target->parent = root;
        target->created = true;
        root->childs.push_back(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(Destroy, Transform*, target);
        ASSERT(target->created);
        auto parent = target->parent;
        ASSERT(parent != nullptr);
        parent->childs.remove(target);

        for (auto child : target->childs)
        {
            auto unit = child->unit;
            unitModule->RecDestroy(context, unit);
        }
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetParent, Transform*, target, Transform*, parent);
        ASSERT(target->created);
        auto oldParent = target->parent;
        ASSERT(oldParent != nullptr);
        oldParent->childs.remove(target);

        parent->childs.push_back(target);
        target->parent = parent;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetPosition, Transform*, target, Vector3f, position);
        target->localPosition = position;
        target->flags.Add(TransformStateFlags::LocalObjectToWorldChanged);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(AddPosition, Transform*, target, Vector3f, position);
        ASSERT(target->created);
        target->localPosition += position;
        target->flags.Add(TransformStateFlags::LocalObjectToWorldChanged);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetRotation, Transform*, target, Vector3f, rotation);
        ASSERT(target->created);
        target->localRotation = Quaternionf::FromEuler(rotation.x, rotation.y, rotation.z);
        target->flags.Add(TransformStateFlags::LocalObjectToWorldChanged);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(CalculateWorldToView, Transform*, target);

        ASSERT(target->created);

        // Find all target ancestors
        worldToViewToCalculate.clear();
        auto searchTransform = target;
        while (searchTransform != root)
        {
            worldToViewToCalculate.push_back(searchTransform);
            searchTransform = searchTransform->parent;
        }

        // Calculate view matrix for each of them
        Transform* parentTransform = nullptr;
        while (!worldToViewToCalculate.empty())
        {
            auto transformToCalculate = worldToViewToCalculate.back();
            worldToViewToCalculate.pop_back();

            if (parentTransform != nullptr)
                transformToCalculate->worldToView = parentTransform->worldToView;
            else
                transformToCalculate->worldToView = Matrix4x4f::indentity;
            transformToCalculate->worldToView.Multiply(Matrix4x4f::Rotate(-transformToCalculate->localRotation));
            transformToCalculate->worldToView.Multiply(Matrix4x4f::Translate(-transformToCalculate->localPosition));

            parentTransform = transformToCalculate;
        }

        DESERIALIZE_METHOD_END;
    }
    return false;
}