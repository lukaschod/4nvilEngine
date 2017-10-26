#include <Rendering\MaterialModule.h>
#include <Graphics\IGraphicsModule.h>

MaterialModule::MaterialModule(uint32_t bufferCount, uint32_t bufferIndexStep) :
	CmdModule(bufferCount, bufferIndexStep)
{
}

void MaterialModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
	ExecuteAfter<ShaderModule>(moduleManager);
}

DECLARE_COMMAND_CODE(CreateMaterial);
const Material* MaterialModule::RecCreateMaterial(const ExecutionContext& context)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto properties = new MaterialProperties();
	auto target = new Material(properties);
	stream.Write(kCommandCodeCreateMaterial);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}

SERIALIZE_METHOD_ARG2(MaterialModule, SetShader, const Material*, const Shader*);
SERIALIZE_METHOD_ARG3(MaterialModule, SetStorage, const Material*, const char*, const Storage*);

bool MaterialModule::ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode)
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
				case kMaterialPropertyTypeStorage:
					auto storage = (const Storage*) property.value;
					graphicsModule->RecSetBuffer(context, pipeline->properties, property.name.c_str(), storage->buffer);
				}
			}
		}
		
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetStorage, Material*, material, const char*, name, const Storage*, storage);
		auto materialProperties = (MaterialProperties*) material->properties;
		SetProperty(materialProperties, name, kMaterialPropertyTypeStorage, (void*) storage);
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
