#pragma once

#include <Core\Tools\Common.h>
#include <Core\Graphics\ITexture.h>
#include <Core\Foundation\PipeModule.h>

namespace Core
{
	struct Sampler;
	class SamplerModule;

	namespace Graphics
	{
		struct ITexture;
		class IGraphicsModule;
	}
}

namespace Core
{
	struct Image
	{
		Image(const Graphics::ITexture* texture)
			: texture(texture)
			, sampler(nullptr)
			, width(texture->width)
			, height(texture->height)
			, created(false)
		{
		}

		const Graphics::ITexture* texture;
		const Sampler* sampler;
		const uint32 width;
		const uint32 height;
		bool created;
	};

	class ImageModule : public PipeModule
	{
	public:
		BASE_IS(PipeModule);

		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Image* AllocateImage(uint32 width, uint32 height) const;

	public:
		void RecCreateImage(const ExecutionContext& context, const Image* target);
		void RecSetSampler(const ExecutionContext& context, const Image* target, const Sampler* sampler);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Image*> images;
		Graphics::IGraphicsModule* graphicsModule;
		SamplerModule* samplerModule;
	};
}