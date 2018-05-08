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
#include <Core/Graphics/Shader.hpp>

namespace Core::Graphics
{
    struct IFilter;
    class IGraphicsModule;
}

namespace Core
{
    struct Shader
    {
        List<const Graphics::IShaderPipeline*> pipelines;
    };

    class ShaderModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

    public:
        const Shader* RecCreateShader(const ExecutionContext& context);
        Void RecSetShaderPipeline(const ExecutionContext& context, const Shader* target, UInt32 index, const Graphics::ShaderPipelineDesc* desc);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        List<Shader*> shaders;
        Graphics::IGraphicsModule* graphicsModule;
    };
}