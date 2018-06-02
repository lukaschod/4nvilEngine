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
#include <Core/Tools/String.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Graphics/VertexLayout.hpp>

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
        ShaderParameter(const Char* name, ShaderParameterType type) : name(name), type(type) {}
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

        ShaderProgram(const UInt8* code, UInt size) :
            available(true),
            code(code),
            size(size)
        {
        }

        Bool available;
        const UInt8* code;
        UInt size;
    };

    struct ShaderPipelineDesc
    {
        List<ShaderParameter> parameters;
        ShaderStates states;
        ShaderProgram programs[SHADER_PROGRAM_COUNT];
        VertexLayout vertexLayout;
        UInt32 varation;
        String name;
        const UInt8* source;
        UInt sourceSize;
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
            values = new Void*[size];
            memset(values, 0, sizeof(Void*) * size);
        }

        const IShaderPipeline* pipeline;
        Void** values;
    };
}