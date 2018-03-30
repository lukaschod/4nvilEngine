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

#include <Core\Foundation\UnitModule.hpp>
#include <Core\Foundation\MemoryModule.hpp>

using namespace Core;

static const char* memoryLabelUnit = "Core::Unit";

void UnitModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
    memoryModule->SetAllocator(memoryLabelUnit, new FixedBlockHeap(sizeof(Unit)));
}

const Unit* UnitModule::AllocateUnit() const
{
    return memoryModule->New<Unit>(memoryLabelUnit);
}

SERIALIZE_METHOD_ARG1(UnitModule, CreateUnit, const Unit*);
SERIALIZE_METHOD_ARG1(UnitModule, Destroy, const Unit*);
SERIALIZE_METHOD_ARG2(UnitModule, AddComponent, const Unit*, const Component*);

bool UnitModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateUnit, Unit*, unit);
        units.push_back(unit);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(Destroy, Unit*, unit);
        units.remove(unit);
        for (auto component : unit->components)
            component->module->RecDestroy(context, component);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(AddComponent, Unit*, unit, Component*, component);
        auto oldUnit = (Unit*) component->unit;
        if (oldUnit != nullptr)
            oldUnit->components.remove(component);
        unit->components.push_back(component);
        component->unit = unit;
        DESERIALIZE_METHOD_END;
    }
    return false;
}