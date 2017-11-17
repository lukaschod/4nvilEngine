#pragma once

#include <Common\EngineCommon.h>
#include <Modules\CmdModule.h>
#include <Graphics\IRenderPass.h>
#include <Graphics\ITexture.h>
#include <Graphics\Shader.h>
#include <Graphics\IBuffer.h>
#include <Graphics\ISwapChain.h>
#include <Math\Rect.h>

struct IView;

struct DrawDesc
{
	const IShaderPipeline* pipeline;
	const IShaderArguments* properties;
	const IBuffer* vertexBuffer;
	uint32_t offset;
	uint32_t size;
};

class IGraphicsModule : public CmdModule
{
public:
	IGraphicsModule(uint32_t bufferCount, uint32_t workersCount);

	virtual const ITexture* RecCreateITexture(const ExecutionContext& context, uint32_t width, uint32_t height) = 0;

	virtual const IFilter* RecCreateIFilter(const ExecutionContext& context, const FilterOptions& options) = 0;

	virtual const IRenderPass* RecCreateIRenderPass(const ExecutionContext& context) = 0;
	virtual void RecSetColorAttachment(const ExecutionContext& context, const IRenderPass* target, uint32_t index, const ColorAttachment& attachment) = 0;
	virtual void RecSetDepthAttachment(const ExecutionContext& context, const IRenderPass* target, const DepthAttachment& attachment) = 0;
	virtual void RecSetViewport(const ExecutionContext& context, const IRenderPass* target, const Viewport& viewport) = 0;
	virtual void RecSetRenderPass(const ExecutionContext& context, const IRenderPass* target) = 0;

	virtual const IShaderPipeline* RecCreateIShaderPipeline(const ExecutionContext& context, const ShaderPipelineDesc* desc) = 0;
	virtual const IShaderArguments* RecCreateIShaderArguments(const ExecutionContext& context, const IShaderPipeline* pipeline) = 0;
	virtual void RecSetTexture(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const ITexture* texture) = 0;
	virtual void RecSetFilter(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IFilter* filter) = 0;
	virtual void RecSetBuffer(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IBuffer* buffer) = 0;

	virtual const IBuffer* RecCreateIBuffer(const ExecutionContext& context, size_t size) = 0;
	virtual void RecUpdateBuffer(const ExecutionContext& context, const IBuffer* target, void* data, size_t size) = 0;

	virtual const ISwapChain* RecCreateISwapChain(const ExecutionContext& context, const IView* view) = 0;
	virtual void RecPresent(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) = 0;
	virtual void RecFinalBlit(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) = 0;

	virtual void RecPushDebug(const ExecutionContext& context, const char* name) = 0;
	virtual void RecPopDebug(const ExecutionContext& context) = 0;

	virtual void RecDraw(const ExecutionContext& context, const DrawDesc& target) = 0;
};