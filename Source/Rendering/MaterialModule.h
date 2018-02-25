#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>
#include <Graphics\Shader.h>

namespace Core::Graphics
{
	class IGraphicsModule;
}

namespace Core
{
	class ShaderModule; struct Shader;
	struct Storage;

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
		Material(const MaterialProperties* properties) :
			shader(nullptr),
			properties(properties)
		{
		}
		const Shader* shader;
		const MaterialProperties* properties;
		List<const MaterialPipeline*> pipelines;
	};

	class MaterialModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Material* RecCreateMaterial(const ExecutionContext& context);
		void RecSetShader(const ExecutionContext& context, const Material* target, const Shader* shader);
		void RecSetStorage(const ExecutionContext& context, const Material* target, const char* name, const Storage* storage);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		inline void SetProperty(MaterialProperties* properties, const char* name, MaterialPropertyType type, void* value);
		inline MaterialProperty* TryFindProperty(MaterialProperties* properties, const char* name);

	private:
		List<Material*> materials;
		List<MaterialProperties*> materialProperties;
		Graphics::IGraphicsModule* graphicsModule;
	};
}