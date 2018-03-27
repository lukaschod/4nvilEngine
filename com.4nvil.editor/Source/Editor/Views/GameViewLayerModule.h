#pragma once

#include <Editor\Views\ViewLayerModule.h>

namespace Editor
{
	class GameViewLayerModule : public ViewLayerModule
	{
	public:
		BASE_IS(ViewLayerModule);
	protected:
		virtual IRenderLoopModule* GetRenderLoop(ModuleManager* moduleManager) override;
	};
}