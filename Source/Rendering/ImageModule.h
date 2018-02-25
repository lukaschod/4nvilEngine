#pragma once

#include <Tools\Common.h>
#include <Graphics\ITexture.h>
#include <Foundation\PipeModule.h>

namespace Core::Graphics
{
	struct ITexture;
	class IGraphicsModule;
}

namespace Core
{
	class SamplerModule; struct Sampler;

	struct Image
	{
		Image(const Graphics::ITexture* texture)
			: texture(texture)
			, sampler(nullptr)
			, width(texture->width)
			, height(texture->height)
		{
		}

		const Graphics::ITexture* texture;
		const Sampler* sampler;
		const uint32 width;
		const uint32 height;
	};

	class ImageModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Image* AllocateImage(uint32 width, uint32 height) const;

	public:
		const Image* RecCreateImage(const ExecutionContext& context, uint32 width, uint32 height, const Image* image = nullptr);
		void RecSetSampler(const ExecutionContext& context, const Image* image, const Sampler* sampler);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Image*> images;
		Graphics::IGraphicsModule* graphicsModule;
		SamplerModule* samplerModule;
	};
}