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
        uint32 offset;
        uint32 size;
    };

    class IGraphicsModule : public PipeModule
    {
    public:
        virtual const IBuffer* AllocateBuffer(size_t size) = 0;
        virtual const ITexture* AllocateTexture(uint32 width, uint32 height) = 0;
        virtual const IFilter* AllocateFilter() = 0;
        virtual const ISwapChain* AllocateSwapChain(const IView* view) = 0;
        virtual const IRenderPass* AllocateRenderPass() = 0;

    public:
        virtual void RecCreateITexture(const ExecutionContext& context, const ITexture* texture) = 0;

        virtual void RecCreateIFilter(const ExecutionContext& context, const IFilter* filter) = 0;

        virtual void RecCreateIRenderPass(const ExecutionContext& context, const IRenderPass* target) = 0;
        virtual void RecSetColorAttachment(const ExecutionContext& context, const IRenderPass* target, uint32 index, const ColorAttachment& attachment) = 0;
        virtual void RecSetDepthAttachment(const ExecutionContext& context, const IRenderPass* target, const DepthAttachment& attachment) = 0;
        virtual void RecSetViewport(const ExecutionContext& context, const IRenderPass* target, const Viewport& viewport) = 0;
        virtual void RecSetRenderPass(const ExecutionContext& context, const IRenderPass* target) = 0;

        virtual const IShaderPipeline* RecCreateIShaderPipeline(const ExecutionContext& context, const ShaderPipelineDesc* desc) = 0;
        virtual const IShaderArguments* RecCreateIShaderArguments(const ExecutionContext& context, const IShaderPipeline* pipeline) = 0;
        virtual void RecSetTexture(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const ITexture* texture) = 0;
        virtual void RecSetFilter(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IFilter* filter) = 0;
        virtual void RecSetBuffer(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IBuffer* buffer) = 0;

        virtual void RecCreateIBuffer(const ExecutionContext& context, const IBuffer* target) = 0;
        virtual void RecSetBufferUsage(const ExecutionContext& context, const IBuffer* target, BufferUsageFlags usage) = 0;
        virtual void RecUpdateBuffer(const ExecutionContext& context, const IBuffer* target, void* data, size_t size) = 0;
        virtual void RecCopyBuffer(const ExecutionContext& context, const IBuffer* src, const IBuffer* dst, size_t size) = 0;

        virtual void RecCreateISwapChain(const ExecutionContext& context, const ISwapChain* target) = 0;
        virtual void RecPresent(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) = 0;
        virtual void RecFinalBlit(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) = 0;

        virtual void RecPushDebug(const ExecutionContext& context, const char* name) = 0;
        virtual void RecPopDebug(const ExecutionContext& context) = 0;

        virtual void RecDraw(const ExecutionContext& context, const DrawDesc& target) = 0;
    };
}