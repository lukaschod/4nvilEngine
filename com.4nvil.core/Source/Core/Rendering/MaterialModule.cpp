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

#include <Core/Foundation/MemoryModule.hpp>
#include <Core/Graphics/IGraphicsModule.hpp>
#include <Core/Rendering/MaterialModule.hpp>
#include <Core/Rendering/ShaderModule.hpp>
#include <Core/Rendering/StorageModule.hpp>

using namespace Core;
using namespace Core::Graphics;

static const Char* memoryLabelMaterial = "Core::Material";

Void Material::Transfer(ITransfer* transfer)
{
    //TRANSFER(shader);
    //TRANSFER(properties);
}

Void MaterialModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
    memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
    memoryModule->SetAllocator(memoryLabelMaterial, new FixedBlockHeap(sizeof(Material)));

    // TODO: Do we really need them?
    ExecuteAfter<ShaderModule>(moduleManager);
    ExecuteAfter<StorageModule>(moduleManager);
}

const Material* MaterialModule::AllocateMaterial() const
{
    auto properties = memoryModule->New<MaterialProperties>(memoryLabelMaterial);
    return memoryModule->New<Material>(memoryLabelMaterial, properties);
}

SERIALIZE_METHOD_ARG1(MaterialModule, CreateMaterial, const Material*);
SERIALIZE_METHOD_ARG1(MaterialModule, Destroy, const Material*);
SERIALIZE_METHOD_ARG2(MaterialModule, SetShader, const Material*, const Shader*);
SERIALIZE_METHOD_ARG3(MaterialModule, SetStorage, const Material*, const Char*, const Storage*);

Bool MaterialModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateMaterial, Material*, target);
        target->created = true;
        materialProperties.push_back((MaterialProperties*)target->properties);
        materials.push_back(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(Destroy, Material*, target);
        NOT_IMPLEMENTED();
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetShader, Material*, material, const Shader*, shader);
        ASSERT(material->created);
        // Delete old pipelines
        if (material->shader != nullptr)
        {
            for (auto pipeline : material->pipelines)
                delete pipeline;
        }

        // Create new pipelines from shader
        material->shader = shader;
        for (auto shaderPipeline : shader->pipelines)
        {
            auto properties = graphicsModule->AllocateShaderArguments(shaderPipeline);
            graphicsModule->RecCreateShaderArguments(context, properties);
            auto pipeline = new MaterialPipeline(shaderPipeline, properties);
            material->pipelines.push_back(pipeline);
        }

        // Reset properties
        for (auto pipeline : material->pipelines)
        {
            for (auto property : material->properties->properties)
            {
                switch (property.type)
                {
                case MaterialPropertyType::Storage:
                    auto storage = (const Storage*) property.value;
                    graphicsModule->RecSetBuffer(context, pipeline->properties, property.name.c_str(), storage->buffer);
                }
            }
        }
        
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(SetStorage, Material*, material, const Char*, name, const Storage*, storage);
        ASSERT(material->created);
        auto materialProperties = (MaterialProperties*) material->properties;
        SetProperty(materialProperties, name, MaterialPropertyType::Storage, (Void*) storage);
        for (auto pipeline : material->pipelines)
        {
            graphicsModule->RecSetBuffer(context, pipeline->properties, name, storage->buffer);
        }
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Void MaterialModule::SetProperty(MaterialProperties* materialProperties, const Char* name, MaterialPropertyType type, Void* value)
{
    auto& properties = materialProperties->properties;
    auto containedProperty = TryFindProperty(materialProperties, name);
    if (containedProperty == nullptr)
    {
        properties.push_back(MaterialProperty(name, type));
        containedProperty = &properties.back();
    }
    containedProperty->value = value;
}

MaterialProperty* MaterialModule::TryFindProperty(MaterialProperties* materialProperties, const Char* name)
{
    auto& properties = materialProperties->properties;
    for (auto& property : properties)
    {
        if (property.name == name)
            return &property;
    }
    return nullptr;
 }
