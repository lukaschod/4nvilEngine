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
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Graphics/IFilter.hpp>

namespace Core::Graphics
{
    class IGraphicsModule;
}

namespace Core
{
    struct Sampler
    {
        Sampler(const Graphics::IFilter* filter) 
            : filter(filter)
            , created(false)
        {}
        const Graphics::IFilter* filter;
        Bool created;
    };

    class SamplerModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        CORE_API virtual Void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        CORE_API const Sampler* AllocateSampler() const;

        inline const Sampler* GetDefaultSampler() const { return samplers[0]; }

    public:
        CORE_API Void RecCreateSampler(const ExecutionContext& context, const Sampler* target);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        List<Sampler*> samplers;
        Graphics::IGraphicsModule* graphicsModule;
    };
}