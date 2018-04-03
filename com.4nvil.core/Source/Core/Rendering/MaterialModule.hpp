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

namespace Core
{
    struct Storage;
    struct Shader;
    class ShaderModule;
    class MemoryModule;
    
    namespace Graphics
    {
        class IGraphicsModule;
    }
}

namespace Core
{
    enum class MaterialPropertyType
    {
        Storage,
    };

    struct MaterialProperty
    {
        MaterialProperty(const char* name, MaterialPropertyType type)
            : name(name)
            , type(type)
            , value(nullptr)
        {
        }
        const String name;
        const MaterialPropertyType type;
        void* value;
    };

    struct MaterialProperties
    {
        List<MaterialProperty> properties;
    };

    struct MaterialPipeline
    {
        MaterialPipeline(const Graphics::IShaderPipeline* pipeline, const Graphics::IShaderArguments* properties) :
            pipeline(pipeline),
            properties(properties)
        {
        }
        const Graphics::IShaderPipeline* const pipeline;
        const Graphics::IShaderArguments* const properties;
    };

    struct Material
    {
        Material(const MaterialProperties* properties) 
            : shader(nullptr)
            , properties(properties)
            , created(false)
        {
        }
        const Shader* shader;
        const MaterialProperties* properties;
        List<const MaterialPipeline*> pipelines;
        bool created;
    };

    class MaterialModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
        virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Material* AllocateMaterial() const;

    public:
        void RecCreateMaterial(const ExecutionContext& context, const Material* material);
        void RecSetShader(const ExecutionContext& context, const Material* target, const Shader* shader);
        void RecSetStorage(const ExecutionContext& context, const Material* target, const char* name, const Storage* storage);

    protected:
        virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        inline void SetProperty(MaterialProperties* properties, const char* name, MaterialPropertyType type, void* value);
        inline MaterialProperty* TryFindProperty(MaterialProperties* properties, const char* name);

    private:
        Graphics::IGraphicsModule* graphicsModule;
        MemoryModule* memoryModule;
        List<Material*> materials;
        List<MaterialProperties*> materialProperties;
    };
}