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

#include <Core/Foundation/SceneModule.hpp>
#include <Core/Foundation/TransformModule.hpp>

using namespace Core;

Void SceneModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    transformModule = ExecuteBefore<TransformModule>(moduleManager);
    unitModule = ExecuteBefore<UnitModule>(moduleManager);
}

const Scene* SceneModule::AllocateScene()
{
    auto scene = new Scene();
    scene->unit = unitModule->AllocateUnit();
    scene->transform = transformModule->AllocateTransform();
    return scene;
}

SERIALIZE_METHOD_ARG1(SceneModule, CreateScene, const Scene*);
SERIALIZE_METHOD_ARG2(SceneModule, SetEnable, const Scene*, Bool);
SERIALIZE_METHOD_ARG2(SceneModule, AddUnit, const Scene*, const Transform*);

Bool SceneModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateScene, Scene*, target);
        ASSERT(!target->created);
        target->created = true;
        unitModule->RecCreateUnit(context, target->unit);
        transformModule->RecCreateTransform(context, target->transform);
        unitModule->RecAddComponent(context, target->unit, target->transform);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetEnable, Scene*, target, Bool, enable);
        ASSERT(target->created);
        unitModule->RecSetEnable(context, target->unit, enable);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(AddUnit, Scene*, target, const Transform*, transform);
        ASSERT(target->created);
        transformModule->RecSetParent(context, transform, target->transform);
        DESERIALIZE_METHOD_END;
    }
    return false;
}
