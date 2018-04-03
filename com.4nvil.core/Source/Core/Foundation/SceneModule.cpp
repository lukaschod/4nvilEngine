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

void SceneModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    transformModule = ExecuteAfter<TransformModule>(moduleManager);
}

const Scene* SceneModule::AllocateScene()
{
    auto scene = new Scene(this);
    scene->transform = transformModule->AllocateTransform();
    return scene;
}

SERIALIZE_METHOD_ARG1(SceneModule, CreateScene, const Scene*);
SERIALIZE_METHOD_ARG2(SceneModule, SetEnable, const Component*, bool);
SERIALIZE_METHOD_ARG2(SceneModule, SetActive, const Component*, bool);

bool SceneModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateScene, Scene*, target);
        ASSERT(!target->created);
        target->created = true;
        auto unit = unitModule->AllocateUnit();
        unitModule->RecCreateUnit(context, unit);
        transformModule->RecCreateTransform(context, target->transform);
        unitModule->RecAddComponent(context, unit, target->transform);
        unitModule->RecAddComponent(context, unit, target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetEnable, Scene*, target, bool, enable);
        ASSERT(target->created);
        target->enabled = enable;
        target->activated &= enable;
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetActive, Scene*, target, bool, activated);
        ASSERT(target->created);
        target->activated = activated;
        DESERIALIZE_METHOD_END;
    }
    return false;
}
