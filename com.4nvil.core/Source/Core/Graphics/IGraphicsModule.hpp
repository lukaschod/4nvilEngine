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
#include <Core/Graphics/IRenderPass.hpp>
#include <Core/Graphics/ITexture.hpp>
#include <Core/Graphics/IFilter.hpp>
#include <Core/Graphics/Shader.hpp>
#include <Core/Graphics/IBuffer.hpp>
#include <Core/Graphics/ISwapChain.hpp>

namespace Core
{
    struct IView;
}

namespace Core::Graphics
{
    struct DrawDesc
    {
        const IShaderPipeline* pipeline;
        const IShaderArguments* properties;
        const IBuffer* vertexBuffer;
        UInt32 offset;
        UInt32 size;
    };

    class IGraphicsModule : public PipeModule
    {
    public:
        virtual const IBuffer* AllocateBuffer(UInt size) = 0;
        virtual const ITexture* AllocateTexture(UInt32 width, UInt32 height) = 0;
        virtual const IFilter* AllocateFilter() = 0;
        virtual const ISwapChain* AllocateSwapChain(const IView* view) = 0;
        virtual const IRenderPass* AllocateRenderPass() = 0;

    public:
        virtual Void RecCreateITexture(const ExecutionContext& context, const ITexture* texture) = 0;

        virtual Void RecCreateIFilter(const ExecutionContext& context, const IFilter* filter) = 0;

        virtual Void RecCreateIRenderPass(const ExecutionContext& context, const IRenderPass* target) = 0;
        virtual Void RecSetColorAttachment(const ExecutionContext& context, const IRenderPass* target, UInt32 index, const ColorAttachment& attachment) = 0;
        virtual Void RecSetDepthAttachment(const ExecutionContext& context, const IRenderPass* target, const DepthAttachment& attachment) = 0;
        virtual Void RecSetViewport(const ExecutionContext& context, const IRenderPass* target, const Viewport& viewport) = 0;
        virtual Void RecSetRenderPass(const ExecutionContext& context, const IRenderPass* target) = 0;

        virtual const IShaderPipeline* RecCreateIShaderPipeline(const ExecutionContext& context, const ShaderPipelineDesc* desc) = 0;
        virtual const IShaderArguments* RecCreateIShaderArguments(const ExecutionContext& context, const IShaderPipeline* pipeline) = 0;
        virtual Void RecSetTexture(const ExecutionContext& context, const IShaderArguments* properties, const Char* name, const ITexture* texture) = 0;
        virtual Void RecSetFilter(const ExecutionContext& context, const IShaderArguments* properties, const Char* name, const IFilter* filter) = 0;
        virtual Void RecSetBuffer(const ExecutionContext& context, const IShaderArguments* properties, const Char* name, const IBuffer* buffer) = 0;

        virtual Void RecCreateIBuffer(const ExecutionContext& context, const IBuffer* target) = 0;
        virtual Void RecSetBufferUsage(const ExecutionContext& context, const IBuffer* target, BufferUsageFlags usage) = 0;
        virtual Void RecUpdateBuffer(const ExecutionContext& context, const IBuffer* target, Void* data, UInt size) = 0;
        virtual Void RecCopyBuffer(const ExecutionContext& context, const IBuffer* src, const IBuffer* dst, UInt size) = 0;

        virtual Void RecCreateISwapChain(const ExecutionContext& context, const ISwapChain* target) = 0;
        virtual Void RecPresent(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) = 0;
        virtual Void RecFinalBlit(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) = 0;

        virtual Void RecPushDebug(const ExecutionContext& context, const Char* name) = 0;
        virtual Void RecPopDebug(const ExecutionContext& context) = 0;

        virtual Void RecDraw(const ExecutionContext& context, const DrawDesc& target) = 0;
    };
}