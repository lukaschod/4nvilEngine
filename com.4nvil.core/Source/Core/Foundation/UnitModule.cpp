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

#include <Core/Foundation/UnitModule.hpp>
#include <Core/Foundation/MemoryModule.hpp>
#include <Core/Foundation/TransformModule.hpp>

using namespace Core;

static const Char* memoryLabelUnit = "Core::Unit";

Void UnitModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
    memoryModule->SetAllocator(memoryLabelUnit, new FixedBlockHeap(sizeof(Unit)));

    // As most components will be used by unit, we should prepare pipe for them too
    List<ComponentModule*> componentModules;
    moduleManager->GetModules<ComponentModule>(componentModules);
    for (auto componentModule : componentModules)
        ExecuteBefore(moduleManager, componentModule);
}

const Unit* UnitModule::AllocateUnit() const
{
    return memoryModule->New<Unit>(memoryLabelUnit);
}

SERIALIZE_METHOD_ARG1(UnitModule, CreateUnit, const Unit*);
SERIALIZE_METHOD_ARG1(UnitModule, Destroy, const Unit*);
SERIALIZE_METHOD_ARG2(UnitModule, AddComponent, const Unit*, const Component*);
SERIALIZE_METHOD_ARG2(UnitModule, SetEnable, const Unit*, Bool);
SERIALIZE_METHOD_ARG1(UnitModule, UpdateActive, const Unit*);

Bool UnitModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateUnit, Unit*, unit);
        units.push_back(unit);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(Destroy, Unit*, unit);
        units.remove(unit);
        for (auto component : unit->components)
            component->module->RecDestroy(context, component); // We can access module here as all of them depend on unitmodule
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetEnable, Unit*, unit, Bool, enable);
        unit->enabled = enable;
        UpdateActive(context, unit);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(UpdateActive, Unit*, unit);
        UpdateActive(context, unit);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(AddComponent, Unit*, unit, Component*, component);
        ASSERT(component->unit == nullptr);

        // Check if it is relation component
        auto relation = dynamic_cast<Transform*>(component);
        if (relation != nullptr)
            unit->relation = relation;

        unit->components.push_back(component);
        component->unit = unit;
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Void UnitModule::UpdateActive(const ExecutionContext& context, Unit* unit)
{
    auto activated = unit->enabled;
    auto relation = unit->relation;
    if (relation != nullptr)
    {
        auto parent = relation->parent;
        if (parent != nullptr && parent->unit != nullptr)
            activated &= parent->unit->activated;
    }

    if (activated == unit->activated)
        return;
    unit->activated = activated;

    if (relation != nullptr)
    {
        for (auto child : relation->childs)
            UpdateActive(context, (Unit*)child->unit);
    }
}
