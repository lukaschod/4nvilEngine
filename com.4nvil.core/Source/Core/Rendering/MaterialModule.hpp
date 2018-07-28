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
        MaterialProperty(const Char* name, MaterialPropertyType type)
            : name(name)
            , type(type)
            , value(nullptr)
        {
        }
        const String name;
        const MaterialPropertyType type;
        Void* value;
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
        Bool created;
    };

    class MaterialModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        CORE_API virtual Void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        CORE_API const Material* AllocateMaterial() const;

    public:
        CORE_API Void RecCreateMaterial(const ExecutionContext& context, const Material* material);
        CORE_API Void RecSetShader(const ExecutionContext& context, const Material* target, const Shader* shader);
        CORE_API Void RecSetStorage(const ExecutionContext& context, const Material* target, const Char* name, const Storage* storage);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        inline Void SetProperty(MaterialProperties* properties, const Char* name, MaterialPropertyType type, Void* value);
        inline MaterialProperty* TryFindProperty(MaterialProperties* properties, const Char* name);

    private:
        Graphics::IGraphicsModule* graphicsModule;
        MemoryModule* memoryModule;
        List<Material*> materials;
        List<MaterialProperties*> materialProperties;
    };
}