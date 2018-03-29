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

#include <Core\Tools\Common.hpp>
#include <Core\Graphics\ITexture.hpp>
#include <Core\Foundation\PipeModule.hpp>

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