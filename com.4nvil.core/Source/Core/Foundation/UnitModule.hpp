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
#include <Core/Tools/Collections/List.hpp>
#include <Core/Foundation/TransfererModule.hpp>

namespace Core
{
    struct Unit;
    struct Transform;
    class ComponentModule;
    class UnitModule;
    class MemoryModule;
}

namespace Core
{
    struct Component : Transferable
    {
        Component() : unit(nullptr) {}

        const Unit* unit;
    };

    class ComponentModule : public TransfererModule
    {
    public:
        BASE_IS(PipeModule);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override
        { 
            base::SetupExecuteOrder(moduleManager); 
            unitModule = ExecuteAfter<UnitModule>(moduleManager);
        }

        virtual Void RecDestroy(const ExecutionContext& context, const Component* unit) {}

    protected:
        UnitModule* unitModule;
    };

    // Container of components
    struct Unit final : Transferable
    {
        IMPLEMENT_TRANSFERABLE(Core, Unit);

        Unit()
            : enabled(true)
            , activated(enabled)
            , relation(nullptr)
        {}

        List<const Component*> components;
        const Transform* relation;
        Bool enabled;
        Bool activated;
    };

    class UnitModule final : public TransfererModule
    {
    public:
        IMPLEMENT_TRANSFERER(Core, Unit);
        BASE_IS(TransfererModule);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Unit* AllocateUnit();

        // Find component from argument component
        template<class T> const T* GetComponent(const Component* target);

        // Find component from argument unit
        template<class T> const T* GetComponent(const Unit* target);

    public:
        Void RecCreateUnit(const ExecutionContext& context, const Unit* target);
        Void RecDestroy(const ExecutionContext& context, const Unit* target);

        // Add component to unit container
        Void RecAddComponent(const ExecutionContext& context, const Unit* target, const Component* component);

        Void RecSetEnable(const ExecutionContext& context, const Unit* target, Bool enable);

        Void RecUpdateActive(const ExecutionContext& context, const Unit* target);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Void UpdateActive(const ExecutionContext& context, Unit* unit);
        ComponentModule* TryGetModule(const TransfererId& id);

    private:
        MemoryModule* memoryModule;
        List<Unit*> units;
        List<ComponentModule*> componentModules;
    };

    template<class T>
    const T* UnitModule::GetComponent(const Component* target)
    {
        auto unit = target->unit;
        for (auto component : unit->components)
        {
            if (dynamic_cast<const T*>(component) != 0)
                return (const T*) component;
        }
        ERROR("Can't find specified component");
        return nullptr;
    }

    template<class T>
    const T* UnitModule::GetComponent(const Unit* target)
    {
        for (auto component : target->components)
        {
            if (dynamic_cast<const T*>(component) != 0)
                return (const T*) component;
        }
        ERROR("Can't find specified component");
        return nullptr;
    }
}