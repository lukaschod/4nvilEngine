#pragma once

#include <Tools\Common.h>
#include <Modules\PipeModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\ITexture.h>
#include <Rendering\SamplerModule.h>

struct Image
{
	Image(const ITexture* texture)
		: texture(texture)
		, sampler(nullptr)
		, width(texture->width)
		, height(texture->height)
	{}

	const ITexture* texture;
	const Sampler* sampler;
	const uint32_t width;
	const uint32_t height;
};

class ImageModule : public PipeModule
{
public:
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Image* AllocateImage(uint32_t width, uint32_t height) const;

public:
	const Image* RecCreateImage(const ExecutionContext& context, uint32_t width, uint32_t height, const Image* image = nullptr);
	void RecSetSampler(const ExecutionContext& context, const Image* image, const Sampler* sampler);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) override;

private:
	List<Image*> images;
	IGraphicsModule* graphicsModule;
	SamplerModule* samplerModule;
};