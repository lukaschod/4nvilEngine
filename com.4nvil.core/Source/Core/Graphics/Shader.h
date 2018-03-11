#pragma once

#include <Core\Tools\Common.h>
#include <Core\Tools\String.h>
#include <Core\Tools\Collections\List.h>
#include <Core\Graphics\VertexLayout.h>

namespace Core::Graphics
{
	enum class ShaderParameterType
	{
		Buffer,
		ConstantBuffer,
		Texture,
		Sampler,
	};

	struct ShaderParameter
	{
		ShaderParameter() {}
		ShaderParameter(const char* name, ShaderParameterType type) : name(name), type(type) {}
		String name;
		ShaderParameterType type;
	};

	enum class ZWrite
	{
		On,
	};

	enum class ZTest
	{
		LEqual,
	};

	struct ShaderStates
	{
		ShaderStates() :
			zWrite(ZWrite::On),
			zTest(ZTest::LEqual)
		{
		}
		ZWrite zWrite;
		ZTest zTest;
	};

	enum class ShaderProgramType
	{
		Vertex,
		Fragment,
	};
#define SHADER_PROGRAM_COUNT 2 // Keep in sync

	struct ShaderProgram
	{
		ShaderProgram() :
			available(false),
			code(nullptr),
			size(0)
		{
		}

		ShaderProgram(const uint8* code, size_t size) :
			available(true),
			code(code),
			size(size)
		{
		}

		bool available;
		const uint8* code;
		size_t size;
	};

	struct ShaderPipelineDesc
	{
		List<ShaderParameter> parameters;
		ShaderStates states;
		ShaderProgram programs[SHADER_PROGRAM_COUNT];
		VertexLayout vertexLayout;
		uint32 varation;
		String name;
		const uint8* source;
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
}