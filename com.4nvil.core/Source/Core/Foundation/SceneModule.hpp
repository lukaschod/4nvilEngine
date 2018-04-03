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

namespace Core
{
    struct Transform;
    class TransformModule;
}

namespace Core
{
    struct Scene : public Component
    {
        Scene(ComponentModule* module) 
            : Component(module)
            , transform(nullptr)
            , created(false)
        {}
        const Transform* transform;
        bool created;
    };

    class SceneModule : public ComponentModule
    {
    public:
        BASE_IS(ComponentModule);

        virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Scene* AllocateScene();

    public:
        void RecCreateScene(const ExecutionContext& context, const Scene* target);
        virtual void RecSetEnable(const ExecutionContext& context, const Component* unit, bool enable) override;
        virtual void RecSetActive(const ExecutionContext& context, const Component* unit, bool activate) override;

    protected:
        virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        TransformModule* transformModule;
    };
}