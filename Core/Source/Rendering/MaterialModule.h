#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>
#include <Rendering\ShaderModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Rendering\StorageModule.h>
#include <Graphics\Shader.h>

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
	{}
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
	MaterialPipeline(const IShaderPipeline* pipeline, const IShaderArguments* properties) : 
		pipeline(pipeline), 
		properties(properties) 
	{}
	const IShaderPipeline* const pipeline;
	const IShaderArguments* const properties;
};

struct Material
{
	Material(const MaterialProperties* properties) :
		shader(nullptr),
		properties(properties)
	{}
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
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, CommandCode commandCode) override;

private:
	inline void SetProperty(MaterialProperties* properties, const char* name, MaterialPropertyType type, void* value);
	inline MaterialProperty* TryFindProperty(MaterialProperties* properties, const char* name);

private:
	List<Material*> materials;
	List<MaterialProperties*> materialProperties;
	IGraphicsModule* graphicsModule;
};