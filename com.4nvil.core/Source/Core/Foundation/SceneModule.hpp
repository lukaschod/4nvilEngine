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
    struct Scene
    {
        Scene() 
            : unit(nullptr)
            , transform(nullptr)
            , created(false)
        {}
        const Unit* unit;
        const Transform* transform;
        Bool created;
    };

    class SceneModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Scene* AllocateScene();

    public:
        Void RecCreateScene(const ExecutionContext& context, const Scene* target);
        Void RecSetEnable(const ExecutionContext& context, const Scene* target, Bool enable);
        Void RecAddUnit(const ExecutionContext& context, const Scene* target, const Transform* transform);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        TransformModule* transformModule;
        UnitModule* unitModule;
    };
}