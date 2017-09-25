#pragma once

#include <Common\EngineCommon.h>
#include <Graphics\VertexLayout.h>
#include <Common\Collections\List.h>
#include <Common\String.h>

enum ShaderParameterType
{
	kShaderParameterTypeBuffer,
	kShaderParameterTypeConstantBuffer,
	kShaderParameterTypeTexture,
	kShaderParameterTypeSampler,
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
	kZWriteOn,
};

enum ZTest
{
	kZTestLEqual,
};

struct ShaderStates
{
	ShaderStates() : 
		zWrite(kZWriteOn), 
		zTest(kZTestLEqual) {}
	ZWrite zWrite;
	ZTest zTest;
};

enum ShaderProgramType
{
	kShaderProgramTypeVertex,
	kShaderProgramTypeFragment,
	kShaderProgramTypeCount,
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
	ShaderProgram programs[kShaderProgramTypeCount];
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