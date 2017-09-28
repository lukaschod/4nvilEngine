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

struct DrawSimple
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

	virtual const ITexture* RecordCreateITexture(const ExecutionContext& context, uint32_t width, uint32_t height) = 0;

	virtual const IFilter* RecordCreateIFilter(const ExecutionContext& context, const FilterOptions& options) = 0;

	virtual const IRenderPass* RecordCreateIRenderPass(const ExecutionContext& context) = 0;
	virtual void RecordSetColorAttachment(const ExecutionContext& context, const IRenderPass* target, uint32_t index, const ColorAttachment& attachment) = 0;
	virtual void RecordSetDepthAttachment(const ExecutionContext& context, const IRenderPass* target, const DepthAttachment& attachment) = 0;
	virtual void RecordSetViewport(const ExecutionContext& context, const IRenderPass* target, const Viewport& viewport) = 0;
	virtual void RecordSetRenderPass(const ExecutionContext& context, const IRenderPass* target) = 0;

	virtual const IShaderPipeline* RecordCreateIShaderPipeline(const ExecutionContext& context, const ShaderPipelineDesc* desc) = 0;
	virtual const IShaderArguments* RecordCreateIShaderArguments(const ExecutionContext& context, const IShaderPipeline* pipeline) = 0;
	virtual void RecordSetTexture(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const ITexture* texture) = 0;
	virtual void RecordSetFilter(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IFilter* filter) = 0;
	virtual void RecordSetBuffer(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IBuffer* buffer) = 0;

	virtual const IBuffer* RecordCreateIBuffer(const ExecutionContext& context, size_t size) = 0;
	virtual void RecordUpdateBuffer(const ExecutionContext& context, const IBuffer* target, void* data, size_t size) = 0;

	virtual const ISwapChain* RecordCreateISwapChain(const ExecutionContext& context, const IView* view) = 0;
	virtual void RecordPresent(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) = 0;
	virtual void RecordFinalBlit(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) = 0;

	virtual void RecordPushDebug(const ExecutionContext& context, const char* name) = 0;
	virtual void RecordPopDebug(const ExecutionContext& context) = 0;

	virtual void RecordBindDrawSimple(const ExecutionContext& context, const DrawSimple& target) = 0;
};