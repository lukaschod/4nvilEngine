#pragma once

#include <Tools\Common.h>
#include <Graphics\VertexLayout.h>
#include <Tools\Collections\List.h>
#include <Tools\String.h>

enum ShaderParameterType
{
	ShaderParameterTypeBuffer,
	ShaderParameterTypeConstantBuffer,
	ShaderParameterTypeTexture,
	ShaderParameterTypeSampler,
};

struct ShaderParameter
{
	ShaderParameter() {}
	ShaderParameter(const char* name, ShaderParameterType type) : name(name), type(type) {}
	String name;
	ShaderParameterType type;
};

enum ZWrite
{
	ZWriteOn,
};

enum ZTest
{
	ZTestLEqual,
};

struct ShaderStates
{
	ShaderStates() : 
		zWrite(ZWriteOn), 
		zTest(ZTestLEqual) {}
	ZWrite zWrite;
	ZTest zTest;
};

enum ShaderProgramType
{
	ShaderProgramTypeVertex,
	ShaderProgramTypeFragment,
	ShaderProgramTypeCount,
};

struct ShaderProgram
{
	ShaderProgram() :
		available(false),
		code(nullptr),
		size(0)
	{
	}

	ShaderProgram(const uint8_t* code, size_t size) :
		available(true),
		code(code),
		size(size)
	{
	}

	bool available;
	const uint8_t* code;
	size_t size;
};

struct ShaderPipelineDesc
{
	List<ShaderParameter> parameters;
	ShaderStates states;
	ShaderProgram programs[ShaderProgramTypeCount];
	VertexLayout vertexLayout;
	uint32_t varation;
	String name;
	const uint8_t* source;
	size_t sourceSize;
};

struct IShaderPipeline : public ShaderPipelineDesc
{
	IShaderPipeline(const ShaderPipelineDesc* desc) 
	{
		parameters = desc->parameters;
		states = desc->states;
		memcpy(programs, desc->programs, sizeof(programs));
		vertexLayout = desc->vertexLayout;
		varation = desc->varation;
		name = desc->name;
		source = desc->source;
		sourceSize = desc->sourceSize;
		delete desc; // Should use comptr
	}
};

struct IShaderArguments
{
	IShaderArguments(const IShaderPipeline* pipeline) : pipeline(pipeline)
	{
		auto size = pipeline->parameters.size();
		values = new void*[size];
		memset(values, 0, sizeof(void*) * size);
	}

	const IShaderPipeline* pipeline;
	void** values;
};