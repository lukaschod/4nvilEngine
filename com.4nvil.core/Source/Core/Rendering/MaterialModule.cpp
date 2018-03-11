#include <Core\Graphics\IGraphicsModule.h>
#include <Core\Rendering\MaterialModule.h>
#include <Core\Rendering\ShaderModule.h>
#include <Core\Rendering\StorageModule.h>

using namespace Core;
using namespace Core::Graphics;

void MaterialModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);

	// TODO: Do we really need them?
	ExecuteAfter<ShaderModule>(moduleManager);
	ExecuteAfter<StorageModule>(moduleManager);
}

DECLARE_COMMAND_CODE(CreateMaterial);
const Material* MaterialModule::RecCreateMaterial(const ExecutionContext& context)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto properties = new MaterialProperties();
	auto target = new Material(properties);
	stream.Write(TO_COMMAND_CODE(CreateMaterial));
	stream.Write(target);
	stream.Align();
	buffer->commandCount++;
	return target;
}

SERIALIZE_METHOD_ARG2(MaterialModule, SetShader, const Material*, const Shader*);
SERIALIZE_METHOD_ARG3(MaterialModule, SetStorage, const Material*, const char*, const Storage*);

bool MaterialModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateMaterial, Material*, target);
		materialProperties.push_back((MaterialProperties*)target->properties);
		materials.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetShader, Material*, material, const Shader*, shader);
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
			auto properties = graphicsModule->RecCreateIShaderArguments(context, shaderPipeline);
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

		DESERIALIZE_METHOD_ARG3_START(SetStorage, Material*, material, const char*, name, const Storage*, storage);
		auto materialProperties = (MaterialProperties*) material->properties;
		SetProperty(materialProperties, name, MaterialPropertyType::Storage, (void*) storage);
		for (auto pipeline : material->pipelines)
		{
			graphicsModule->RecSetBuffer(context, pipeline->properties, name, storage->buffer);
		}
		DESERIALIZE_METHOD_END;
	}
	return false;
}

void MaterialModule::SetProperty(MaterialProperties* materialProperties, const char* name, MaterialPropertyType type, void* value)
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

MaterialProperty* MaterialModule::TryFindProperty(MaterialProperties* materialProperties, const char* name)
{
	auto& properties = materialProperties->properties;
	for (auto& property : properties)
	{
		if (property.name == name)
			return &property;
	}
	return nullptr;
 }
