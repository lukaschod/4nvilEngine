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

#include <Core\Tools\Common.hpp>
#include <Core\Tools\Collections\List.hpp>
#include <Core\Foundation\PipeModule.hpp>

namespace Core
{
    struct Unit;
    class ComponentModule;
    class UnitModule;
    class MemoryModule;
}

namespace Core
{
    struct Component
    {
        Component(ComponentModule* module)
            : module(module)
            , unit(nullptr)
            , enabled(true)
            , activated(enabled)
        {
        }
        virtual ~Component() {}

        const Unit* unit;
        ComponentModule* module;
        bool enabled;
        bool activated;
    };

    class ComponentModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual void SetupExecuteOrder(ModuleManager* moduleManager) override
        { 
            base::SetupExecuteOrder(moduleManager); 
            unitModule = ExecuteAfter<UnitModule>(moduleManager);
        }

        virtual void RecDestroy(const ExecutionContext& context, const Component* unit) = 0;
        virtual void RecSetEnable(const ExecutionContext& context, const Component* unit, bool enable) {}
        virtual void RecSetActive(const ExecutionContext& context, const Component* unit, bool activate) {}

    protected:
        UnitModule* unitModule;
    };

    // Container of components
    struct Unit
    {
        Unit()
            : enabled(true)
            , activated(enabled)
        {}
        List<const Component*> components;
        bool enabled;
        bool activated;
    };

    class UnitModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Unit* AllocateUnit() const;

        // Find component from argument component
        template<class T> const T* GetComponent(const Component* target);

        // Find component from argument unit
        template<class T> const T* GetComponent(const Unit* target);

    public:
        void RecCreateUnit(const ExecutionContext& context, const Unit* target);
        void RecDestroy(const ExecutionContext& context, const Unit* target);

        // Add component to unit container
        void RecAddComponent(const ExecutionContext& context, const Unit* target, const Component* component);

        void RecSetEnable(const ExecutionContext& context, const Unit* target, bool enable);

        void RecSetActive(const ExecutionContext& context, const Unit* target, bool activate);

    protected:
        virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        MemoryModule* memoryModule;
        List<Unit*> units;
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